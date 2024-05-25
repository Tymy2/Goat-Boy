#include "Mmu.h"
#include <stdint.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>

MMU::MMU(){
	this->memory = (uint8_t *)malloc(0x10000);
	//zero the memory to prevent unwanted values;
	for(int i = 0; i < 0x10000; i++){
		this->memory[i] = 0;
	}
}

MMU::~MMU(){
	free(this->memory);
}

uint8_t MMU::read(uint16_t addr){
	return this->memory[addr];
}

uint16_t MMU::read_16(uint16_t addr){
	if(addr == 0xffff){
		return this->read(addr) + (this->read(addr+1) << 8);
	}
	return *(uint16_t *)(this->memory + addr);
}

void MMU::write(uint16_t addr, uint8_t value){
	this->memory[addr] = value;
}

void MMU::write_16(uint16_t addr, uint16_t value){
	if(addr == 0xffff){
		this->write(addr, value & 0xff);
		this->write(addr+1, value >> 8);
		return;
	}
	*(uint16_t *)(this->memory + addr) = value;
}

void MMU::load_from_file(char const * filepath, uint16_t offset){
	std::ifstream ifs(filepath, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

	if(pos > (0x10000 - offset)){
		std::cout << "file size doesnt match memory! it wont be loaded." << std::endl;
		ifs.close();
		return;
	}

    std::vector<char>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);
    
	std::copy(result.begin()+offset, result.end(), this->memory+offset);

	ifs.close();
}
