#include <SDL2/SDL.h>

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

	public:
		GUI();
		~GUI();
		void handle_events(bool * keep_runing);
		void update_pixels(uint32_t * pixels);
};

#endif
