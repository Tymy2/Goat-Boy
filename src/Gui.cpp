#include <SDL2/SDL.h>
#include "headers/Gui.h"
#include <iostream>
#include <chrono>

GUI::GUI(){
	
}

void GUI::init(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		return;
	}
	this->window = SDL_CreateWindow(this->title_buffer, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width * 4, this->height * 4, 0);
	this->renderer = SDL_CreateRenderer(this->window, -1, 0);
	SDL_RenderSetLogicalSize(this->renderer, this->width, this->height);
 	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, this->width, this->height);
	this->keyboard_ptr = SDL_GetKeyboardState(this->numkeys);
	this->prev_timestamp = std::chrono::high_resolution_clock::now();
}

GUI::~GUI(){
	SDL_DestroyTexture(this->texture);
}

void GUI::handle_events(){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		if (event.type == SDL_QUIT) {
      		this->keep_running = false;
			return;
   		}
		if(event.type == SDL_KEYDOWN){
			if(event.key.keysym.scancode == SDL_SCANCODE_D){
				*this->debug_enabled = !*this->debug_enabled;
			}
		}
	}
}

void GUI::manage_framerate(){
	this->frame_counter++;
	auto now = std::chrono::high_resolution_clock::now();
	if(std::chrono::duration_cast<std::chrono::seconds>(now - this->prev_timestamp).count() >= 1){
		sprintf_s(this->title_buffer, 256, "GoatBoy - %d fps", this->frame_counter);
		SDL_SetWindowTitle(this->window, this->title_buffer);

		this->frame_counter = 0;
		this->prev_timestamp = now;
	}
}

void GUI::update_keyboard_state(){
	SDL_PumpEvents();
}

void GUI::update_pixels(uint32_t * pixels){
    SDL_LockTexture(this->texture, NULL, (void * *) &this->pixelpointer, &this->pitch);
	for(int y = 0; y < this->height; y++){
		for(int x = 0; x < this->width; x++){
			*(this->pixelpointer++) = *(pixels++);
		}
	}
	SDL_UnlockTexture(this->texture);
	SDL_RenderClear(this->renderer);
 	SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
  	SDL_RenderPresent(this->renderer);
	this->manage_framerate();
}
