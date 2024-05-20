#pragma once
#include <stdint.h>

#ifndef MMU_HEADER
#define MMU_HEADER
class MMU{
	public:
		uint8_t * memory;

		MMU();
		~MMU();
		uint8_t read(uint16_t);
		uint16_t read_16(uint16_t);
		void write(uint16_t, uint8_t);
		void write_16(uint16_t, uint16_t);
		void load_from_file(char const * filepath, uint16_t offset);
};
#endif
