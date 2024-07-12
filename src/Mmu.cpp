#include "headers/Mmu.h"
#include "MBC/headers/MBC.h"
#include "MBC/headers/MBC_0.h"
#include "MBC/headers/MBC_3.h"
#include <stdint.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>

#define MODE_3 3
#define MODE_2 2

#define ROM_BANK_0_END 0x4000
#define SWITCHABLE_ROM_BANK_END 0x8000
#define VRAM_END 0xA000
#define WORK_RAM_END 0xE000
#define EXTERNAL_RAM_END 0xC000
#define ECHO_RAM_END 0xFE00
#define OAM_END 0xFEA0
#define UNUSED_RAM_END 0xFF00

MMU::MMU(){
	this->memory = (uint8_t *)malloc(0x10000);
}

MMU::~MMU(){
	free(this->memory);
	delete this->mbc;
}

uint8_t MMU::read(uint16_t addr){
	if(addr < ROM_BANK_0_END){
		return this->mbc->read_rom(addr);
	}
	if(addr < SWITCHABLE_ROM_BANK_END){
		return this->mbc->read_rom(addr);
	}
	if(addr < VRAM_END){
		if(*this->ppu_mode_ptr == MODE_3){
			return 0xff;
		}
		return this->memory[addr];
	}
	if(addr < EXTERNAL_RAM_END){
		return this->mbc->read_ram(addr);
	}
	if(addr < WORK_RAM_END){
		return this->memory[addr];
	}
	if(addr < ECHO_RAM_END){
		return this->memory[0xc000 + (WORK_RAM_END - addr)];
	}
	if(addr < OAM_END){
		if((*this->ppu_mode_ptr) == MODE_2 || (*this->ppu_mode_ptr) == MODE_3 ){
			return 0xff;
		}
		return this->memory[addr];
	}
	if(addr < UNUSED_RAM_END){
		return 0xff;
	}
	return this->memory[addr];
}

uint16_t MMU::read_16(uint16_t addr){
	return this->read(addr) + (this->read(addr+1) << 8);
}

void MMU::write(uint16_t addr, uint8_t value){
	if(addr < SWITCHABLE_ROM_BANK_END){
		this->mbc->write(addr, value);
		return;
	}
	if(addr < VRAM_END){
		if(*this->ppu_mode_ptr != MODE_3){
			this->memory[addr] = value;
		}
		return;
	}
	if(addr < EXTERNAL_RAM_END){
		this->mbc->write(addr, value);
		return;
	}
	if(addr < WORK_RAM_END){
		this->memory[addr] = value;
		return;
	}
	if(addr < ECHO_RAM_END){
		this->memory[0xc000 + (WORK_RAM_END - addr)] = value;
		return;
	}
	if(addr < OAM_END){
		if((*this->ppu_mode_ptr) != MODE_2 && (*this->ppu_mode_ptr) != MODE_3 ){
			this->memory[addr] = value;
		}
		return;
	}
	if(addr < UNUSED_RAM_END){
		return;
	}
	// OAM TRANSFER
	if(addr == 0xff46){
		uint16_t source = (value << 8) & 0xdf00;
		for(uint16_t i = 0; i <= 0x9f; i++){
			this->write(0xfe00+i, read(source+i));
		}
	}
	this->memory[addr] = value;
}

void MMU::write_16(uint16_t addr, uint16_t value){
	this->write(addr, value & 0xff);
	this->write(addr+1, value >> 8);
}

void MMU::load_from_file(char const * filepath){
	std::ifstream ifs(filepath, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char>  result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

	uint8_t MBC_type = result.at(0x0147);
	if(MBC_type == 0x00){
		this->mbc = new MBC_0();
	}else if(MBC_type >= 0x0f && MBC_type <= 0x13){
		this->mbc = new MBC_3();
	}else{
		printf("MBC type not supported, defaulting to MBC 0.\n");
		this->mbc = new MBC_0();
	}

	this->mbc->load_rom(result);

	ifs.close();
}
