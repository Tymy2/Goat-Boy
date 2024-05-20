#include "Device.h"
#include "instructions.h"
#include <iostream>

void Device::init(){
	this->cpu.device = this;
	setup_instructions(this);
}

// TODO change implementation, it is really bad to output to console, speed goes really down
void debug_print(Device * device){
	uint8_t flags = device->cpu.r[0x1];
	std::cout << "op_code: ";
	printf("%02X", int(device->mmu.read(device->cpu.pc)));
	if(device->mmu.read(device->cpu.pc) == 0xcb){
		printf(" %02X", int(device->mmu.read(device->cpu.pc+1)));
	}
	printf("\tRegisters: [ A: %02x B: %02x C: %02x D: %02x E: %02x H: %02x L: %02x ] Flags: [ Z: %d N: %d H: %d C: %d ] PC: %04x SP: %04x\n", 
		device->cpu.r[0x0],
		device->cpu.r[0x2],
		device->cpu.r[0x3],
		device->cpu.r[0x4],
		device->cpu.r[0x5],
		device->cpu.r[0x6],
		device->cpu.r[0x7],
		flags >> 7,
		(flags >> 6) & 0x1,
		(flags >> 5) & 0x1,
		(flags >> 4) & 0x1,
		device->cpu.pc,
		device->cpu.sp
	);
}

void Device::tick(){
	if(this->debug_enabled) debug_print(this); 
	this->cpu.decode_and_execute();
}
