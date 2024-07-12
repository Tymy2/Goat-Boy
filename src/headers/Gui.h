#pragma once
#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>

#ifndef GUI_H
#define GUI_H

class GUI{
	private:
		SDL_Window * window;
		SDL_Renderer * renderer;
		SDL_Texture * texture;
		const int width = 160;
		const int height = 144;
		int pitch = 640;
		uint32_t * pixelpointer = NULL;
		int * numkeys = NULL;
		char title_buffer[256] = {"GoatBoy"};
		int frame_counter = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> prev_timestamp;

	public:
		GUI();
		~GUI();
		void handle_events();
		void update_pixels(uint32_t * pixels);
		void update_keyboard_state();
		void manage_framerate();
		const uint8_t * keyboard_ptr = NULL;
		bool keep_running = true;
};

#endif
