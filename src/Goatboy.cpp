#include "headers/Gui.h"
#include "headers/Device.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <filesystem>

#define FRAMERATE_60 0
#define FRAMERATE_UNCAPPED 1

struct Config {
	bool debug_enabled = false;
	std::string rom_path;
	std::string savefile_path;
	uint8_t framerate = FRAMERATE_60;
};

class GoatBoy{
public:
	GUI gui;
	Device device;
	uint16_t delay_ms = 0;

	void init(){
		this->gui.init();
		this->device.init(this->gui.keyboard_ptr);
		this->gui.debug_enabled = &this->device.debug_enabled;
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
				SDL_Delay(this->delay_ms);
			}
		}
		this->on_stop();
	}
	
	void on_stop(){
		this->device.mmu.save_savefile();
	}

	void set_framerate(uint8_t framerate){
		switch(framerate){
			case FRAMERATE_60:
				this->delay_ms = 15;
				break;
			case FRAMERATE_UNCAPPED:
				this->delay_ms = 0;
				break;
			default:
				this->delay_ms = 15;
				break;
		}
	}

};

bool parse_arguments(int argc, char * argv[], Config * config){
	if(argc < 2){
		printf("[ERROR] Missing arguments, use \"--help\" for more information.\n");
		return false;
	}	
	for(int i = 1; i < argc; i++){
		if(std::strcmp(argv[i], "--debug") == 0){
			config->debug_enabled = true;
			continue;
		}
		if(std::strcmp(argv[i], "--fps-uncapped") == 0){
			config->framerate = FRAMERATE_UNCAPPED;
			continue;
		}
		if(std::strcmp(argv[i], "--gamefile") == 0){
			if(i >= argc){
				printf("[ERROR] Missing game file path.\n");
				return false;
			}
			config->rom_path = argv[++i];
			continue;
		}
		if(std::strcmp(argv[i], "--savefile") == 0){
			if(i >= argc){
				printf("[ERROR] Missing game save file path.\n");
				return false;
			}
			config->savefile_path = argv[++i];
			continue;
		}
		if(std::strcmp(argv[i], "--help") == 0){
			printf("%-30sEnables debug mode to console.\n", "--debug");
			printf("%-30sFramerate has no cap.\n", "--fps-uncapped");
			printf("%-30sFile of the game you want to play.\n", "--gamefile <path>");
			printf("%-30s[WIP] File where the game data is saved.\n", "--savefile <path>");
			printf("%-30sShows this information.\n", "--help");
			return false;
		}
		
		// Alone argument is probable to be the game file, so for now we check if it exists, and if not then it might be a typo
		if(std::filesystem::exists(argv[i])){
			config->savefile_path = argv[i];
			continue;
		}

		printf("[ERROR] Unkown argument or non existing game file \"%s\"\n", argv[i]);
		printf("Use argument \"--help\" to get more information.\n");
		return false;
	}
	return true;
}

void apply_config(Config * config, GoatBoy * gb){
	gb->device.debug_enabled = config->debug_enabled;
	gb->set_framerate(config->framerate);
	if(!config->rom_path.empty()){
		gb->device.mmu.load_rom(config->rom_path.c_str());
	}
	if(!config->savefile_path.empty()){
		gb->device.mmu.load_savefile(config->savefile_path.c_str());
	}
}

int main(int argc, char * argv[]){
	Config config;
	if(parse_arguments(argc, argv, &config)){
		GoatBoy gb;
		apply_config(&config, &gb);
		gb.init();
		gb.run();		
	}
	printf("exiting...\n");
	return 0;
}
