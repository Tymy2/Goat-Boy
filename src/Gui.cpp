#include <SDL2/SDL.h>
#include "headers/Gui.h"
#include <iostream>
#include <chrono>

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
uint8_t handle_keys(uint8_t old_state, const uint8_t * keyboard_ptr, uint8_t * joypad_state){
	bool dpad_selected = !((old_state >> 4) & 1);
	bool buttons_selected = !((old_state >> 5) & 1);
	uint8_t new_state = old_state & 0x30;
	
	if(old_state >= 0x30){
		return 0x3f;
	}

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
	
	// Joypad interrupt
	if((old_state & 0xf) != (new_state & 0xf)){
		*(joypad_state + 0x0f) |= 0b10000;		
	}

	return new_state;
}

int limiter = 0;
void GUI::handle_events(bool * keep_running, uint8_t * joypad_state){
	*joypad_state = handle_keys(*joypad_state, this->keyboard_ptr, joypad_state);
	if(limiter++ >= 100000){
		limiter = 0;
		SDL_PumpEvents();
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT) {
       			*keep_running = false;
				return;
   			}
		}
	}
}

int frames = 0;
auto old = std::chrono::high_resolution_clock::now();
void manage_framerate(SDL_Window * window){
	frames++;
	auto now = std::chrono::high_resolution_clock::now();
	if(std::chrono::duration_cast<std::chrono::seconds>(now-old).count() >= 1){
		char buffer[256];
		sprintf_s(buffer, 256, "GoatBoy - %d fps", frames);
		SDL_SetWindowTitle(window, buffer);
		frames = 0;
		old = now;
	}
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
	manage_framerate(this->window);
}
