#include "headers/Gui.h"
#include "headers/Device.h"
#include <SDL2/SDL.h>
#include <iostream>

class GoatBoy{
public:
	GUI gui;
	Device device;

	void init(int argc, char * argv[]){
		this->device.mmu.load_from_file(argv[0x1]);
		this->device.init(this->gui.keyboard_ptr);
	}
	
	void run(){
		while(this->gui.keep_running){
			if(this->device.keep_running){
				this->device.tick();
			}
			if(this->device.ppu.frame_ready){
				this->gui.update_pixels(this->device.ppu.pixels);
				this->gui.update_keyboard_state();
				this->gui.handle_events();
				SDL_Delay(0);
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
