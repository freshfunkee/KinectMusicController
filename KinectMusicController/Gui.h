#ifndef GUI_H
#define GUI_H

class CVImage;

#include <SDL\SDL.h>
#include <SDL\SDL_ttf.h>
#include <SDL\SDL_image.h>
#include <opencv2\opencv.hpp>
#undef main

class Gui
{
public:
	Gui( CVImage* );
	~Gui();

	void initialize();
	void drawSurfaceFromMatrix(IplImage&);
	void drawTempoString(std::string&);
	void displayFrame();
	void run();

private:
	SDL_Surface *screen_, *surface_, *text_;
	TTF_Font *font_;
	SDL_Surface *textSurface_, *pauseSurface_;
	CVImage *cvImage_;
	IplImage frame_;

	void drawGui();
};

#endif