#pragma once
#include "MBC.h"

#ifndef MBC_3_H
#define MBC_3_H
class MBC_3: public MBC{
public:
	uint8_t * rom = NULL;
	uint8_t * ram = NULL;
	uint8_t rom_bank_number = 1;
	uint8_t ram_bank_number = 0;
	bool enable_ram = false;
	MBC_3();
	~MBC_3();
	void load_rom(std::vector<char> rom_data);
	void load_ram(std::vector<char> ram_data);
	uint8_t read_rom(uint16_t addr);
	uint8_t read_ram(uint16_t addr);
	void write(uint16_t addr, uint8_t value);
	void save_data();
};
#endif
