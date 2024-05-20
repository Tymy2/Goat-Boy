#include "Cpu.h"
#include <stdint.h>
#include "Device.h"
#include <iostream>

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

void CPU::decode_and_execute(){
	(* this->instructions[this->fetch()])(this->device);
}

void CPU::set_flags(uint8_t mask, uint8_t values){
	uint8_t flags = this->r[0x1];
	mask <<= 4;
	values <<= 4;
	//								 <-----------error_fix------------->
	this->r[0x1] = (flags & ~mask) | (values ^ (values & (mask ^ 0xF0)));
}
