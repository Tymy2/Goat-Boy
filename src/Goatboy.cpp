#include "headers/Gui.h"
#include "headers/Device.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <filesystem>

struct Config {
	bool debug_enabled = false;
	std::string rom_path;
	std::string savefile_path;
};

class GoatBoy{
public:
	GUI gui;
	Device device;

	void init(){
		this->gui.init();
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
		this->on_stop();
	}
	
	void on_stop(){
		this->device.mmu.save_savefile();
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
			printf("--debug%50s\n", "Enables debug mode to console.");
			printf("--gamefile <path>%50s\n", "File of the game you want to play.");
			printf("--savefile <path>%50s\n", "[WIP] File where the game data is saved.");
			printf("--help%50s\n", "Shows this information.");
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
