#include <SDL2/SDL.h>
#include "headers/Gui.h"
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
	uint32_t * pixelpointer = NULL;
	int pitch = 4 * this->width;
	SDL_Texture* buffer = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, this->width, this->height);
    SDL_LockTexture(buffer, NULL, (void * *) &pixelpointer, &pitch);
	for(int y = 0; y < this->height; y++){
		for(int x = 0; x < this->width; x++){
			*(pixelpointer++) = *(pixels++);
		}
	}
	SDL_UnlockTexture(buffer);
	SDL_RenderClear(this->renderer);
 	SDL_RenderCopy(this->renderer, buffer, NULL, NULL);
  	SDL_RenderPresent(this->renderer);
	SDL_DestroyTexture(buffer);
}
