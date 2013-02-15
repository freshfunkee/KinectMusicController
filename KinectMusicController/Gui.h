#ifndef GUI_H
#define GUI_H

#include <SDL\SDL.h>
#include <SDL\SDL_ttf.h>
#include <SDL\SDL_image.h>
#undef main

class Gui
{
public:
	Gui();
	~Gui();

	void initialize();
	//void drawTempoString(std::string&);
	void displayFrame(long **);
	void run();

private:
	SDL_Surface *screen_, *surface_, *text_, *jointImg_;
	TTF_Font *font_;
	SDL_Surface *textSurface_, *pauseSurface_;

	void drawGui();
};

#endif