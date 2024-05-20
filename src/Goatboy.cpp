#include "Gui.h"
#include "Device.h"
#include <SDL2/SDL.h>
#include <iostream>

class GoatBoy{
public:
	GUI gui;
	Device device;
	bool keep_running = true;

	void init(int argc, char * argv[]){
		this->gui.init();
		this->device.init();
		this->device.mmu.load_from_file(".\\roms\\dmg_boot.bin", 0x00); // custom bootup rom
		this->device.mmu.load_from_file(argv[0x1], 0x100); // load rom
	}

	void run(){
		while(this->keep_running){
			if(this->device.keep_running){
				this->device.tick();
			}
			this->gui.handle_events(&this->keep_running);
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

	gb.device.debug_enabled = true;

	gb.run();
	return 0;
}
