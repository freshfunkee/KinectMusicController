#ifndef GUI_H
#define GUI_H

#include <string>
#include <SDL\SDL.h>
#include <SDL\SDL_ttf.h>
#include <SDL\SDL_image.h>
#undef main

#define GUI_SCREEN_SIZE_X		640
#define GUI_SCREEN_SIZE_Y		580
#define GUI_SCREEN_OFFSET_Y		50

#define GUI_BUTTON_COUNT		5
#define GUI_BUTTON_SIZE			50

#define GUI_CAMERA_SIZE_X		640
#define GUI_CAMERA_SIZE_Y		480

#define GUI_BUTTON_PAUSE_INDEX	0
#define GUI_BUTTON_PAUSE_POS_X	270
#define GUI_BUTTON_PAUSE_POS_Y	450

#define GUI_BUTTON_FILTER_INDEX	1
#define GUI_BUTTON_FILTER_POS_X	50
#define GUI_BUTTON_FILTER_POS_Y 100

#define GUI_BUTTON_2_INDEX		2
#define GUI_BUTTON_2_POS_X		10
#define GUI_BUTTON_2_POS_Y		200

#define GUI_BUTTON_3_INDEX		3
#define GUI_BUTTON_3_POS_X		10
#define GUI_BUTTON_3_POS_Y		300

#define GUI_BUTTON_4_INDEX		4
#define GUI_BUTTON_4_POS_X		50
#define GUI_BUTTON_4_POS_Y		400

#define GUI_STRING_TEMPO_SIZE	14
#define GUI_STRING_TEMPO_POS_X	380
#define GUI_STRING_TEMPO_POS_Y	25

typedef enum {
	eButtonIdle,
	eButtonHover,
	eButtonHover1000,
	eButtonActive,
	eButtonInactive
} ButtonState;

typedef enum {
	eGuiTempo,
	eGuiFilter
} GuiState;

class Gui
{
public:
	Gui();
	~Gui();

	void initialize();
	void displayFrame(long **);
	void displayFilter();
	void displayTempo();
	void setLowpass(long&);
	void setHighpass(long&);
	void setTempoString(const long&);
	void setButtonStates(ButtonState*);
	GuiState getGuiState() { return guiState_; }
	void run();

private:
	SDL_Surface *screen_, *skelSurface_, *guiSurface_, *text_, *jointImg_;
	TTF_Font *font_, *font2_;
	SDL_Surface *textSurface_, *pauseSurface_, *dspSurface_, filterSurface_;

	std::string tempo_, filter_;
	long lowpass_, highpass_;

	ButtonState *buttonStates_;
	GuiState guiState_;
	void drawGui();
};

#endif