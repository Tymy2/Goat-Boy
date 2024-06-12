#include <SDL2/SDL.h>
#include "headers/Gui.h"
#include <iostream>

GUI::GUI(){
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		return;
	}
	this->window = SDL_CreateWindow("GoatBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width*4, this->height*4, 0);
	this->renderer = SDL_CreateRenderer(this->window, -1, 0);
			
	SDL_RenderSetLogicalSize(this->renderer, this->width, this->height);
 	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, this->width, this->height);
	this->keyboard_ptr = SDL_GetKeyboardState(this->numkeys);
}

GUI::~GUI(){
	SDL_DestroyTexture(this->texture);
}

#define START_DOWN_BIT 0b1000
#define SELECT_UP_BIT  0b0100
#define B_LEFT_BIT     0b0010
#define A_RIGHT_BIT    0b0001
uint8_t handle_keys(uint8_t old_state, const uint8_t * keyboard_ptr){
	bool dpad_selected = !((old_state >> 4) & 1);
	bool buttons_selected = !((old_state >> 5) & 1);
	uint8_t new_state = old_state & 0x30;
	
	if(dpad_selected){
		new_state |= !(keyboard_ptr[SDL_SCANCODE_DOWN])  << 3;
		new_state |= !(keyboard_ptr[SDL_SCANCODE_UP])    << 2;
		new_state |= !(keyboard_ptr[SDL_SCANCODE_LEFT])  << 1;
		new_state |= !(keyboard_ptr[SDL_SCANCODE_RIGHT]) << 0;
	}

	if(buttons_selected){
		new_state |= !(keyboard_ptr[SDL_SCANCODE_ESCAPE])     << 3;
		new_state |= !(keyboard_ptr[SDL_SCANCODE_TAB])        << 2;
		new_state |= !(keyboard_ptr[SDL_SCANCODE_BACKSPACE])  << 1;
		new_state |= !(keyboard_ptr[SDL_SCANCODE_SPACE])      << 0;
	}

	return new_state;
}

void GUI::handle_events(bool * keep_running, uint8_t * joypad_state){
	SDL_PumpEvents();
	SDL_Event event;
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
        *keep_running = false;
		return;
    }
	*joypad_state = handle_keys(*joypad_state, this->keyboard_ptr);
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
}
