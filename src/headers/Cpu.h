#pragma once
#include <stdint.h>
class Device;

#ifndef CPU_H
#define CPU_H
class CPU {
	public:
		union {
			uint8_t r[8]; 	// A, F, B, C, D, E, H, L
			uint16_t rr[4] = {0, 0, 0, 0}; // AF, BC, DE, HL
		};
		uint16_t pc = 0x0;
		uint16_t sp = 0x0;
		bool IME = false;
		bool halt_mode = false;
		void (* instructions[0x100])(Device *) = {};
		void (* cb_instructions[0x100])(Device *) = {};
		uint16_t index_cycles = 0; // used to get bot cpu cycles and ppu dots
		Device * device = NULL;
		int tac_clock = 0;
		int div_clock = 0;

		uint8_t fetch();
		uint16_t fetch_16();
		void handle_interrupts();
		void tick();
		void handle_timers();
		void set_flags(uint8_t, uint8_t);
};
#endif
