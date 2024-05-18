#include <SDL2/SDL.h>

#ifndef GUI_H
#define GUI_H

class GUI{
private:
	SDL_Window * window;
	SDL_Renderer * renderer;
	int width = 160;
	int height = 144;

public:
	void init();
	void update();
	void handle_events(bool * keep_runing);
};

#endif
