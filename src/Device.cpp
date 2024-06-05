#include "headers/Device.h"
#include "headers/instructions.h"
#include <iostream>

void setup_starting_state(Device * device);
void debug_print(Device * device);

void Device::init(){
	this->cpu.device = this;
	setup_instructions(this);
	setup_cb_instructions(this);
	setup_starting_state(this);
	this->ppu.memory = this->mmu.memory; // setup memory pointer for ppu
}

void Device::tick(){
	if(this->debug_enabled) debug_print(this); 
	this->cpu.decode_and_execute();
	this->ppu.tick(this->cpu.index_cycles);
}

// TODO change implementation, it is really bad to output to console, speed goes really down
void debug_print(Device * device){
	uint8_t flags = device->cpu.r[0x0];
	uint8_t op_code = device->mmu.read(device->cpu.pc);
	uint8_t cb_code = device->mmu.read(device->cpu.pc+1);
	
	if(op_code == 0xcb){
		printf("Op_code: %02x %02x ", op_code, cb_code);
	}else{
		printf("Op_code: %02x %2s ", op_code, "");
	}
	printf("Registers: [ A: %02x B: %02x C: %02x D: %02x E: %02x H: %02x L: %02x ] Flags: [ Z: %d N: %d H: %d C: %d ] PC: %04x SP: %04x\n", 
		device->cpu.r[0x1],
		device->cpu.r[0x3],
		device->cpu.r[0x2],
		device->cpu.r[0x5],
		device->cpu.r[0x4],
		device->cpu.r[0x7],
		device->cpu.r[0x6],
		flags >> 7,
		(flags >> 6) & 0x1,
		(flags >> 5) & 0x1,
		(flags >> 4) & 0x1,
		device->cpu.pc,
		device->cpu.sp
	);
}

void setup_starting_state(Device * device){
	device->cpu.rr[0x00] = 0x01B0;
	device->cpu.rr[0x01] = 0x0013;
	device->cpu.rr[0x02] = 0x00D8;
	device->cpu.rr[0x03] = 0x014D;
	device->cpu.pc = 0x100;
	device->cpu.sp = 0xFFFE;
  	device->mmu.memory[0xFF05] = 0x00;
  	device->mmu.memory[0xFF06] = 0x00;
  	device->mmu.memory[0xFF07] = 0x00;
	device->mmu.memory[0xFF10] = 0x80;
 	device->mmu.memory[0xFF11] = 0xBF;
    device->mmu.memory[0xFF12] = 0xF3;
	device->mmu.memory[0xFF14] = 0xBF;
    device->mmu.memory[0xFF16] = 0x3F;
    device->mmu.memory[0xFF17] = 0x00;
    device->mmu.memory[0xFF19] = 0xBF;
    device->mmu.memory[0xFF1A] = 0x7F;
    device->mmu.memory[0xFF1B] = 0xFF;
    device->mmu.memory[0xFF1C] = 0x9F;
    device->mmu.memory[0xFF1E] = 0xBF;
    device->mmu.memory[0xFF20] = 0xFF;
    device->mmu.memory[0xFF21] = 0x00;
    device->mmu.memory[0xFF22] = 0x00;
    device->mmu.memory[0xFF23] = 0xBF;
    device->mmu.memory[0xFF24] = 0x77;
    device->mmu.memory[0xFF25] = 0xF3;
    device->mmu.memory[0xFF26] = 0xF1;
	device->mmu.memory[0xFF40] = 0x91;
    device->mmu.memory[0xFF42] = 0x00;
    device->mmu.memory[0xFF43] = 0x00;
    device->mmu.memory[0xFF45] = 0x00;
    device->mmu.memory[0xFF47] = 0xFC;
    device->mmu.memory[0xFF48] = 0xFF;
    device->mmu.memory[0xFF49] = 0xFF;
    device->mmu.memory[0xFF4A] = 0x00;
    device->mmu.memory[0xFF4B] = 0x00;
    device->mmu.memory[0xFFFF] = 0x00;
}
