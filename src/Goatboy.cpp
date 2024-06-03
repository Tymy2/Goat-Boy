#include "headers/Gui.h"
#include "headers/Device.h"
#include <SDL2/SDL.h>
#include <iostream>

class GoatBoy{
public:
	GUI gui;
	Device device;
	bool keep_running = true;

	void init(int argc, char * argv[]){
		this->device.init();
		this->device.mmu.load_from_file(argv[0x1], 0x00);
		this->device.ppu.memory = this->device.mmu.memory; // setup memory pointer for ppu
		this->setup_starting_state();
	}

	void setup_starting_state(){
		this->device.cpu.rr[0x00] = 0x01B0;
		this->device.cpu.rr[0x01] = 0x0013;
		this->device.cpu.rr[0x02] = 0x00D8;
		this->device.cpu.rr[0x03] = 0x014D;
		this->device.cpu.pc = 0x100;
		this->device.cpu.sp = 0xFFFE;
	  	this->device.mmu.memory[0xFF05] = 0x00;
	  	this->device.mmu.memory[0xFF06] = 0x00;
	  	this->device.mmu.memory[0xFF07] = 0x00;
		this->device.mmu.memory[0xFF10] = 0x80;
	 	this->device.mmu.memory[0xFF11] = 0xBF;
	    this->device.mmu.memory[0xFF12] = 0xF3;
		this->device.mmu.memory[0xFF14] = 0xBF;
	    this->device.mmu.memory[0xFF16] = 0x3F;
	    this->device.mmu.memory[0xFF17] = 0x00;
	    this->device.mmu.memory[0xFF19] = 0xBF;
	    this->device.mmu.memory[0xFF1A] = 0x7F;
	    this->device.mmu.memory[0xFF1B] = 0xFF;
	    this->device.mmu.memory[0xFF1C] = 0x9F;
	    this->device.mmu.memory[0xFF1E] = 0xBF;
	    this->device.mmu.memory[0xFF20] = 0xFF;
	    this->device.mmu.memory[0xFF21] = 0x00;
	    this->device.mmu.memory[0xFF22] = 0x00;
	    this->device.mmu.memory[0xFF23] = 0xBF;
	    this->device.mmu.memory[0xFF24] = 0x77;
	    this->device.mmu.memory[0xFF25] = 0xF3;
	    this->device.mmu.memory[0xFF26] = 0xF1;
		this->device.mmu.memory[0xFF40] = 0x91;
	    this->device.mmu.memory[0xFF42] = 0x00;
	    this->device.mmu.memory[0xFF43] = 0x00;
	    this->device.mmu.memory[0xFF45] = 0x00;
	    this->device.mmu.memory[0xFF47] = 0xFC;
	    this->device.mmu.memory[0xFF48] = 0xFF;
	    this->device.mmu.memory[0xFF49] = 0xFF;
	    this->device.mmu.memory[0xFF4A] = 0x00;
	    this->device.mmu.memory[0xFF4B] = 0x00;
	    this->device.mmu.memory[0xFFFF] = 0x00;
	}
	
	void run(){
		while(this->keep_running){
			if(this->device.keep_running){
				this->device.tick();
			}
			this->gui.handle_events(&this->keep_running);
			if(this->device.ppu.clock == 0){
				this->gui.update_pixels(this->device.ppu.pixels);
			}
		}
	}

};

int main(int argc, char * argv[]){
	if(argc != 2){
		std::cout << "Wrong number of arguments! quitting..." << std::endl;
		return 0;
	}
	GoatBoy gb;
	gb.init(argc, argv);

	gb.device.debug_enabled = false;

	gb.run();
	return 0;
}
