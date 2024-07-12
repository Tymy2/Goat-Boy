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
#define DIV_ADDR 0xff04
#define TIMA_ADDR 0xff05
#define TMA_ADDR 0xff06
#define TAC_ADDR 0xff07
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

const uint16_t TAC_SPEEDS[4] = { 256, 4, 16, 64 };
void CPU::handle_timers(){
	this->div_clock += CYCLES[this->index_cycles];
	while(this->div_clock >= 64){
		this->div_clock -= 64;
		this->device->mmu.memory[DIV_ADDR]++;
	}
	uint8_t TAC = this->device->mmu.memory[TAC_ADDR];
	bool TAC_enabled = (TAC & 0b100) == 0b100;
		
	if(!TAC_enabled){
		return;
	}
	
	uint16_t clock_select = TAC_SPEEDS[(TAC & 0b11)];
	this->tac_clock += CYCLES[this->index_cycles];
	while(this->tac_clock >= clock_select){
		this->tac_clock -= clock_select;
		this->device->mmu.memory[TIMA_ADDR]++;
		if(this->device->mmu.memory[TIMA_ADDR] == 0){
			this->device->mmu.memory[TIMA_ADDR] = this->device->mmu.memory[TMA_ADDR];
			this->device->mmu.memory[IF_ADDR] |= 0b100;
		}
	}
}

void CPU::tick(){
	if(!this->halt_mode){
		uint8_t op_code = this->fetch();
		this->index_cycles = op_code;

		(* this->instructions[op_code])(this->device); // execute instruction
		
		this->handle_timers();
		this->handle_interrupts();
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
