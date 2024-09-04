#pragma once
#include <stdint.h>
#include <vector>
#include <iostream>
#include <string>

#ifndef MBC_H
#define MBC_H
class MBC{
public:
	std::string savefile_path;
	virtual void load_rom(std::vector<char> rom_data) = 0;
	virtual void load_ram(std::vector<char> ram_data) = 0;
	virtual uint8_t read_rom(uint16_t addr) = 0;
	virtual uint8_t read_ram(uint16_t addr) = 0;
	virtual void write(uint16_t addr, uint8_t value) = 0;
	virtual void save_data() = 0;
};
#endif
