#include <SDL2/SDL.h>
#include "Gui.h"

void GUI::init(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		return;
	}
	this->window = SDL_CreateWindow("GoatBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width*4, this->height*4, 0);
	this->renderer = SDL_CreateRenderer(this->window, -1, 0);
			
	SDL_RenderSetLogicalSize(this->renderer, this->width, this->height);
}

void GUI::handle_events(bool * keep_running){
	SDL_Event event;
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
        *keep_running = false;
    }
}

void GUI::update(){
	// TODO
}
