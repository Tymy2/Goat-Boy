#include <iostream>
#include "headers/Joypad.h"
#include <SDL2/SDL_scancode.h>

void Joypad::init(uint8_t * if_ptr, uint8_t * joypad_ptr, const uint8_t * keyboard_ptr){
	this->if_ptr = if_ptr;
	this->joypad_ptr = joypad_ptr;
	this->keyboard_ptr = keyboard_ptr;
}

#define START_DOWN_BIT 0b1000
#define SELECT_UP_BIT  0b0100
#define B_LEFT_BIT     0b0010
#define A_RIGHT_BIT    0b0001
#define JOYPAD_INTERRUPT_BIT 0b10000
#define DPAD_SELECTED(state) (!((state >> 4) & 1))
#define BUTTONS_SELECTED(state) (!((state >> 5) & 1))
void Joypad::update(){
	uint8_t old_state = *this->joypad_ptr & 0x3f;
	// no mode has been selected
	if(old_state >= 0x30){
		*this->joypad_ptr = 0x3f;
		return;
	}
	uint8_t new_state = old_state & 0x30;
	
	if( DPAD_SELECTED(old_state) ){
		new_state |= !(this->keyboard_ptr[SDL_SCANCODE_DOWN])  << 3;
		new_state |= !(this->keyboard_ptr[SDL_SCANCODE_UP])    << 2;
		new_state |= !(this->keyboard_ptr[SDL_SCANCODE_LEFT])  << 1;
		new_state |= !(this->keyboard_ptr[SDL_SCANCODE_RIGHT]) << 0;
	}

	if( BUTTONS_SELECTED(old_state) ){
		new_state |= !(this->keyboard_ptr[SDL_SCANCODE_ESCAPE])     << 3;
		new_state |= !(this->keyboard_ptr[SDL_SCANCODE_TAB])        << 2;
		new_state |= !(this->keyboard_ptr[SDL_SCANCODE_BACKSPACE])  << 1;
		new_state |= !(this->keyboard_ptr[SDL_SCANCODE_SPACE])      << 0;
	}
	
	*this->joypad_ptr = new_state;
	// Joypad interrupt
	if(old_state != new_state){
		*this->if_ptr |= JOYPAD_INTERRUPT_BIT;
	}
}

