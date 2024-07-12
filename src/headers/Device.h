#include "Cpu.h"
#include "Mmu.h"
#include "Ppu.h"
#include "Joypad.h"

#ifndef DEVICE_H
#define DEVICE_H
class Device{
	public:
		CPU cpu;
		MMU mmu;
//		APU apu;
		PPU ppu;
		Joypad joypad;
		bool keep_running = true;
		bool debug_enabled = false;

		void init(const uint8_t * keyboard_ptr);
		void tick();
};
#endif
