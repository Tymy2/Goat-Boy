#include "Cpu.h"
#include "Mmu.h"

#ifndef DEVICE_H
#define DEVICE_H
class Device{
	public:
		CPU cpu;
		MMU mmu;
//		APU apu;
//		PPU ppu;
		bool keep_running = true;
		bool debug_enabled = false;

		void init();
		void tick();
};
#endif
