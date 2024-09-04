#include "headers/MBC_0.h"
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

#define ROM_SIZE 0x8000
#define RAM_SIZE 0x2000

MBC_0::MBC_0(){
	this->rom = (uint8_t *)malloc(ROM_SIZE);
	this->ram = (uint8_t *)malloc(RAM_SIZE);
}
		
MBC_0::~MBC_0(){
	free(this->rom);
	free(this->ram);
}

void MBC_0::load_rom(std::vector<char> rom_data){
	std::copy(rom_data.begin(), rom_data.begin() + ROM_SIZE, rom);
}

void MBC_0::load_ram(std::vector<char> ram_data){
	std::copy(ram_data.begin(), ram_data.begin() + RAM_SIZE, ram);
}

uint8_t MBC_0::read_rom(uint16_t addr){
	if(addr < SWITCHABLE_ROM_BANK_END){
		return this->rom[addr];
	}
	return 0xff;
}

uint8_t MBC_0::read_ram(uint16_t addr){
	if(addr >= VRAM_END && addr < EXTERNAL_RAM_END){
		return this->ram[addr - 0xA000];
	}
	return 0xff;
}

void MBC_0::write(uint16_t addr, uint8_t value){
	if(addr >= VRAM_END && addr < EXTERNAL_RAM_END){
		this->ram[addr - 0xA000] = value;
	}
}

void MBC_0::save_data(){
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
