#include "Gui.h"
#include "Device.h"
#include <SDL2/SDL.h>

class GoatBoy{
private:
	GUI gui;
	Device device;
	bool keep_running = true;

public:
	void init(){
		this->gui.init();
		this->device.init();
	}

	void run(){
		while(this->keep_running){
			this->device.tick();
			this->gui.handle_events(&this->keep_running);
		}
	}
};

int main(int argc, char * argv[]){
	GoatBoy gb;
	gb.init();
	gb.run();
	return 0;
}
