#pragma once
#include "MBC.h"

#ifndef MBC_0_H
#define MBC_0_H
class MBC_0: public MBC{
public:
	uint8_t * rom = NULL;
	uint8_t * ram = NULL;
	MBC_0();
	~MBC_0();
	void load_rom(std::vector<char> rom_data);
	void load_ram(std::vector<char> ram_data);
	uint8_t read_rom(uint16_t addr);
	uint8_t read_ram(uint16_t addr);
	void write(uint16_t addr, uint8_t value);
	void save_data();
};
#endif
