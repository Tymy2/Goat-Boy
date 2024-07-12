#pragma once

#ifndef JOYPAD_HEADER
#define JOYPAD_HEADER
class Joypad{
	private:
		uint8_t * if_ptr = NULL;
		uint8_t * joypad_ptr = NULL;
		const uint8_t * keyboard_ptr = NULL;
	public:
		void init(uint8_t * if_ptr, uint8_t * joypad_ptr, const uint8_t * keyboard_ptr);
		void update();
};
#endif
