#include "headers/Cpu.h"
#include "headers/Device.h"
#include <stdint.h>
#include <iostream>

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

void CPU::decode_and_execute(){
	if(!this->halt_mode){
		this->device->mmu.memory[JOYPAD_ADDR] |= 0b1111; // no buttons pressed
		this->handle_interrupts();
		uint8_t op_code = this->fetch();
		this->index_cycles = op_code;
		(* this->instructions[op_code])(this->device);
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
