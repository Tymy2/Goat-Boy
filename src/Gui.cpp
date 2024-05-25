#include <SDL2/SDL.h>
#include "Gui.h"
#include <iostream>

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

void GUI::update_pixels(uint32_t * pixels){
	SDL_RenderClear(this->renderer);
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	for(int y = 0; y < this->height; y++){
		for(int x = 0; x < this->width; x++){
			uint32_t pixel = *(pixels++);
			r = pixel >> 24;
			g = (pixel >> 16) & 0xff;
			b = (pixel >> 8) & 0xff;
			SDL_SetRenderDrawColor(this->renderer, r, g, b, 0xff);
			SDL_RenderDrawPoint(this->renderer, x, y);
		}
	}
	SDL_RenderPresent(this->renderer);
}
