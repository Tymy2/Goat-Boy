#include "headers/Cpu.h"
#include "headers/Device.h"
#include <stdint.h>
#include <iostream>
#include <chrono>
#include "headers/cycles.h"

#define F 0
#define IE_ADDR 0xffff
#define IF_ADDR 0xff0f
#define JOYPAD_ADDR 0xff00
#define VBLANK_INTERRUPT_ADDR 0x40
#define LCD_INTERRUPT_ADDR 0x48
#define TIMER_INTERRUPT_ADDR 0x50
#define SERIAL_INTERRUPT_ADDR 0x58
#define JOYPAD_INTERRUPT_ADDR 0x60
#define INTERRUPT_PINS (this->device->mmu.memory[IE_ADDR] & this->device->mmu.memory[IF_ADDR])
#define INTERRUPT_PENDING (INTERRUPT_PINS == 0)

uint8_t CPU::fetch(){
	uint8_t value = this->device->mmu.read(this->pc);
	this->pc++;
	return value;
}

uint16_t CPU::fetch_16(){
	uint16_t value = this->device->mmu.read_16(this->pc);
	this->pc += 2;
	return value;
}

void CPU::handle_interrupts(){	
	if(!this->IME){
		return;
	}

	uint8_t pins = INTERRUPT_PINS;
	if(pins == 0){
		return;
	}
	
	// TODO fix cycles for intrrupts
	for(uint8_t bit = 0; bit < 5; bit++){
		bool pin_bit_set = (pins >> bit) & 0x1;
		if(pin_bit_set){
			this->sp -= 2;
			this->device->mmu.write_16(this->sp, this->pc);
			this->pc = VBLANK_INTERRUPT_ADDR + (0x8*bit); // we shift by 0x08 times corresponding to the interrupt address
			this->IME = false;
			this->device->mmu.memory[IF_ADDR] ^= (0x1 << bit); // clear the bit
			break;
		}
	}
}

int clock_divider = 0;
int clock_tac = 0;
uint16_t TAC_SPEEDS[4] = { 256, 4, 16, 64 };
void CPU::handle_timers(){
	clock_divider += CYCLES[this->index_cycles];
	while(clock_divider >= 64){
		clock_divider -= 64;
		this->device->mmu.memory[0xff04]++;
	}
	uint8_t TAC = this->device->mmu.memory[0xff07];
	bool TAC_enabled = (TAC & 0b100) == 0b100;
	uint16_t clock_select = TAC_SPEEDS[(TAC & 0b11)];
	if(TAC_enabled){
		clock_tac += CYCLES[this->index_cycles];
		while(clock_tac >= clock_select){
			clock_tac -= clock_select;
			this->device->mmu.memory[0xff05]++;
			if(this->device->mmu.memory[0xff05] == 0){
				this->device->mmu.memory[0xff05] = this->device->mmu.memory[0xff06];
				this->device->mmu.memory[0xff0f] |= 0b100;
			}
		}
	}
}

long ops = 0;
auto old_ops = std::chrono::high_resolution_clock::now();
void manage_ops(){
	ops++;
	auto now = std::chrono::high_resolution_clock::now();
	if(std::chrono::duration_cast<std::chrono::seconds>(now-old_ops).count() >= 1){
		printf("Operations_s: %d\n", ops);
		ops = 0;
		old_ops = now;
	}
}


void CPU::decode_and_execute(){
	if(!this->halt_mode){
		uint8_t op_code = this->fetch();
		this->index_cycles = op_code;
		(* this->instructions[op_code])(this->device);
		this->handle_timers();
		this->handle_interrupts();
		manage_ops();
		return;
	}
	this->halt_mode = INTERRUPT_PENDING;
}

void CPU::set_flags(uint8_t mask, uint8_t values){
	uint8_t flags = this->r[F];
	mask <<= 4;
	values <<= 4;
	//							   <-----------error_fix------------->
	this->r[F] = (flags & ~mask) | (values ^ (values & (mask ^ 0xF0)));
}
