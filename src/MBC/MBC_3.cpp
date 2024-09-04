#include "headers/MBC_3.h"
#include <iostream>
#include <fstream>

#define ROM_BANK_0_END 0x4000
#define SWITCHABLE_ROM_BANK_END 0x8000
#define VRAM_END 0xA000
#define WORK_RAM_END 0xE000
#define EXTERNAL_RAM_END 0xC000
#define ECHO_RAM_END 0xFE00
#define OAM_END 0xFEA0
#define UNUSED_RAM_END 0xFF00

#define RAM_SIZE 0x8000

MBC_3::MBC_3(){
	this->rom = (uint8_t *)malloc(ROM_BANK_0_END*128);
	this->ram = (uint8_t *)malloc(RAM_SIZE);
}

MBC_3::~MBC_3(){
	free(this->rom);
	free(this->ram);
}

void MBC_3::load_rom(std::vector<char> rom_data){
	std::copy(rom_data.begin(), rom_data.end(), rom);
}

void MBC_3::load_ram(std::vector<char> ram_data){
	std::copy(ram_data.begin(), ram_data.begin() + RAM_SIZE, ram);
}

uint8_t MBC_3::read_rom(uint16_t addr){
	if(addr < ROM_BANK_0_END){
		return this->rom[addr];
	}
	if(addr < SWITCHABLE_ROM_BANK_END){
		return this->rom[(this->rom_bank_number * ROM_BANK_0_END) + (addr - ROM_BANK_0_END) ];
	}
	return 0xff;
}

uint8_t MBC_3::read_ram(uint16_t addr){
	if(addr >= VRAM_END && addr < EXTERNAL_RAM_END){
		return this->ram[(this->ram_bank_number * 0x2000) + (addr - VRAM_END)];
	}
	return 0xff;
}


void MBC_3::write(uint16_t addr, uint8_t value){
	if(addr < 0x2000){
		if(value == 0x00){
			this->enable_ram = false;
		}else if(value == 0x0A){
			this->enable_ram = true;
		}
		return;
	}
	if(addr < ROM_BANK_0_END){
		value &= 0x7f;
		if(value == 0){
			value = 1;
		}
		this->rom_bank_number = value;
		return;
	}
	if(addr < 0x6000){
		if(value <= 0x03){
			this->ram_bank_number = value;
		}
		return;
	}
	if(addr >= VRAM_END && addr < EXTERNAL_RAM_END){
		this->ram[(this->ram_bank_number * 0x2000) + (addr - VRAM_END)] = value;
		return;
	}
}

void MBC_3::save_data(){
	if(this->savefile_path.empty()){
		return;
	}
	
	std::ofstream savefile_stream(this->savefile_path);
	
	if(savefile_stream.is_open()){
		for(int i = 0; i < RAM_SIZE; i++){
			savefile_stream << this->ram[i];
		}
		savefile_stream.close();
	}else{
		printf("[ERROR] Couldnt write save file.");
	}
}
