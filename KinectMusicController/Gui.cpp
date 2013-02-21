#include "Gui.h"

#include <sstream>

Gui::Gui()
{
	screen_ = NULL;
	skelSurface_ = NULL;
	guiSurface_ = NULL;
	tempo_ = "Tempo: ";
	filter_ = "Filter";

	buttonStates_ = NULL;
	guiState_ = eGuiTempo;

	highpass_ = 200;
	lowpass_ = 600;
}

Gui::~Gui()
{
	if(screen_ != NULL)
	{
		SDL_FreeSurface(screen_);
	}
	TTF_CloseFont(font_);
	TTF_Quit();
	SDL_Quit();
}

void Gui::initialize()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	jointImg_ = IMG_Load("Joint.png");
	font_ = TTF_OpenFont("Syncopate-Bold.ttf", GUI_STRING_TEMPO_SIZE);
	font2_ = TTF_OpenFont("Syncopate-Regular.ttf", GUI_STRING_TEMPO_SIZE);
}

void Gui::displayFilter()
{
	guiState_ = eGuiFilter;
}

void Gui::displayTempo()
{
	guiState_ = eGuiTempo;
}

void Gui::displayFrame(long **skelMatrix)
{
	if (!screen_) 
    {
        screen_ = SDL_SetVideoMode(GUI_SCREEN_SIZE_X, GUI_SCREEN_SIZE_Y, 0, SDL_HWSURFACE | SDL_NOFRAME);
        if (!screen_) 
        {
            fprintf(stderr, "SDL: could not set video mode - exiting\n");
            exit(1);
        }
    }

	guiSurface_ = SDL_CreateRGBSurface(SDL_HWSURFACE, GUI_SCREEN_SIZE_X, GUI_SCREEN_SIZE_Y, 16, 0, 0, 0, 0);
	SDL_FillRect( guiSurface_, NULL, 0x93C47D00);

	skelSurface_ = SDL_CreateRGBSurface(SDL_HWSURFACE, GUI_CAMERA_SIZE_X, GUI_CAMERA_SIZE_Y, 16, 0, 0, 0, 0);
	SDL_FillRect( skelSurface_, NULL, 0x93C47D00);
	
	for(int i=0;i<20;i++)
	{
		SDL_Rect posRect = { skelMatrix[i][0], skelMatrix[i][1], 0, 0 };
		SDL_BlitSurface( jointImg_, 0, skelSurface_, &posRect );
	}

	SDL_Rect mainScreen = { 0, GUI_SCREEN_OFFSET_Y, 0, 0 };
	if(SDL_BlitSurface(skelSurface_, 0, guiSurface_, &mainScreen)==-1)
	{
		printf("SDL Error! %s ",SDL_GetError());
		if(skelSurface_ == NULL)
			printf("surface_ is NULL\n");

		if(guiSurface_ == NULL)
			printf("screen_ is NULL\n");
	}

	SDL_FreeSurface(skelSurface_);
	drawGui();

	if(SDL_BlitSurface(guiSurface_, 0, screen_, 0)==-1)
	{
		printf("SDL Error! %s ",SDL_GetError());
		if(guiSurface_ == NULL)
			printf("surface_ is NULL\n");

		if(screen_ == NULL)
			printf("screen_ is NULL\n");
	}

	SDL_FreeSurface(guiSurface_);

    SDL_Flip(screen_);
}

void Gui::drawGui()
{
	SDL_Rect pauseLocation = { GUI_BUTTON_PAUSE_POS_X, GUI_BUTTON_PAUSE_POS_Y, 0, 0 };

	switch(buttonStates_[GUI_BUTTON_PAUSE_INDEX]) 
	{
	case eButtonIdle:
		pauseSurface_ = IMG_Load("pause_button_idle.png");
		break;
	case eButtonHover:
		pauseSurface_ = IMG_Load("pause_button_hover1.png");
		break;
	case eButtonHover1000:
		pauseSurface_ = IMG_Load("pause_button_hover2.png");
		break;
	case eButtonActive:
		pauseSurface_ = IMG_Load("pause_button_active.png");
		break;
	}

	SDL_BlitSurface(pauseSurface_, NULL, guiSurface_, &pauseLocation);
	SDL_FreeSurface(pauseSurface_);

	for(int i=GUI_BUTTON_FILTER_INDEX;i<GUI_BUTTON_COUNT;i++)
	{
		switch(buttonStates_[i]) 
		{
		case eButtonIdle:
			dspSurface_ = IMG_Load("button_idle.png");
			break;
		case eButtonHover:
			dspSurface_ = IMG_Load("button_hover1.png");
			break;
		case eButtonHover1000:
			dspSurface_ = IMG_Load("button_hover2.png");
			break;
		case eButtonActive:
			dspSurface_ = IMG_Load("button_active.png");
			break;
		case eButtonInactive:
			dspSurface_ = IMG_Load("button_inactive.png");
		break;
		}

		if(i == GUI_BUTTON_FILTER_INDEX || i == GUI_BUTTON_4_INDEX)
		{
			SDL_Rect dspLocation = { GUI_BUTTON_FILTER_POS_X, GUI_BUTTON_FILTER_POS_Y*i, 0, 0 };
			SDL_BlitSurface(dspSurface_, NULL, guiSurface_, &dspLocation);
			SDL_FreeSurface(dspSurface_);
		}
		else
		{
			SDL_Rect dspLocation = { GUI_BUTTON_2_POS_X, GUI_BUTTON_FILTER_POS_Y*i, 0, 0 };
			SDL_BlitSurface(dspSurface_, NULL, guiSurface_, &dspLocation);
			SDL_FreeSurface(dspSurface_);
		}
	}

	switch(guiState_)
	{
	case eGuiTempo:
		break;
	case eGuiFilter:
		SDL_Surface *cutoffSurface = IMG_Load("button_inactive.png");
		SDL_Rect lowLocation = { lowpass_-25, 180, 0, 0 };
		SDL_Rect highLocation = { highpass_-25, 180, 0, 0 };
		SDL_BlitSurface(cutoffSurface, NULL, guiSurface_, &lowLocation);
		SDL_BlitSurface(cutoffSurface, NULL, guiSurface_, &highLocation);

		SDL_FreeSurface(cutoffSurface);

		SDL_Surface *cutoffLineSurface = IMG_Load("cutoff_line.png");
		SDL_Rect lowLineLocation = { 600, 100, 0, 0 };
		SDL_Rect highLineLocation = { 200, 100, 0, 0 };
		SDL_BlitSurface(cutoffLineSurface, NULL, guiSurface_, &lowLineLocation);
		SDL_BlitSurface(cutoffLineSurface, NULL, guiSurface_, &highLineLocation);

		SDL_FreeSurface(cutoffLineSurface);

		SDL_Color foregroundColor = { 0, 0, 0 };
		SDL_Rect textLocationFilter = { 110, 95, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, filter_.c_str(),
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationFilter);

		SDL_FreeSurface(textSurface_);

		SDL_Rect textLocationHighpass = { 180, 450, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, "20Hz",
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationHighpass);

		SDL_FreeSurface(textSurface_);

		SDL_Rect textLocationLowpass = { 580, 450, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, "20kHz",
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationLowpass);

		SDL_FreeSurface(textSurface_);
		break;
	}

	SDL_Color foregroundColor = { 0, 0, 0 };
	textSurface_ = TTF_RenderText_Blended(font_, tempo_.c_str(),
		foregroundColor);

	SDL_Rect textLocation = { GUI_STRING_TEMPO_POS_X, GUI_STRING_TEMPO_POS_Y, 0, 0 };

	SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation);
	SDL_FreeSurface(textSurface_);
}

void Gui::setTempoString(const long &tempo)
{
	std::stringstream ss;
	float relSpeed = (((float)tempo)/95.3) * 100;
	ss.precision(0);
	ss << "Tempo: " << tempo << " BPM (" << (int)relSpeed << "%)";
	tempo_ = ss.str();
}

void Gui::setButtonStates(ButtonState *buttonStates)
{
	buttonStates_ = buttonStates;
}

void Gui::setHighpass(long &cutoff)
{
	highpass_ = cutoff;
}

void Gui::setLowpass(long &cutoff)
{
	lowpass_ = cutoff;
}

