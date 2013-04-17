#include "Gui.h"

#include <sstream>

Gui::Gui()
{
	screen_ = NULL;
	skelSurface_ = NULL;
	guiSurface_ = NULL;

	tempo_ = "Tempo: ";
	filter_ = "Filter";
	echostr_ = "Echo";
	flangestr_ = "Flange";
	tremolostr_ = "Tremolo";

	buttonStates_ = NULL;
	guiState_ = eGuiTempo;

	highpass_ = 200;
	lowpass_ = 600;

	flangeDepth_ = 402;
	tremoloRate_ = 402;

	curTimeMs_ = 0;
	beatCount_ = 1;

	echoStates_ = new bool[3];

	for(int i=0; i<3; i++)
	{
		echoStates_[i] = false;
	}
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

	delete [] echoStates_;
}

void Gui::initialize()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	jointImg_ = IMG_Load("joint.png");
	jointImgHand_ = IMG_Load("joint_hand.png");
	font_ = TTF_OpenFont("Syncopate-Bold.ttf", GUI_STRING_TEMPO_SIZE);
	font2_ = TTF_OpenFont("Syncopate-Regular.ttf", GUI_STRING_TEMPO_SIZE);
}

void Gui::displayFilter()
{
	guiState_ = eGuiFilter;
}

void Gui::displayEcho()
{
	guiState_ = eGuiEcho;
}

void Gui::displayTempo()
{
	guiState_ = eGuiTempo;
}

void Gui::displayFlange()
{
	guiState_ = eGuiFlange;
}

void Gui::displayTremolo()
{
	guiState_ = eGuiTremolo;
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
	SDL_FillRect( guiSurface_, NULL, SDL_MapRGB(guiSurface_->format,147,196,125) );

	skelSurface_ = SDL_CreateRGBSurface(SDL_HWSURFACE, GUI_CAMERA_SIZE_X, GUI_CAMERA_SIZE_Y, 16, 0, 0, 0, 0);
	SDL_FillRect( skelSurface_, NULL, SDL_MapRGB(guiSurface_->format,147,196,125) );
	
	for(int i=0;i<20;i++)
	{
		SDL_Rect posRect = { skelMatrix[i][0], skelMatrix[i][1], 0, 0 };

		if(i == 7 || i == 11)
			SDL_BlitSurface( jointImgHand_, 0, skelSurface_, &posRect );
		else
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
		{
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
		}
		break;
	case eGuiEcho:
		{
		SDL_Surface *echoRectSurface;
		
		if(echoStates_[0])
		{
			echoRectSurface = IMG_Load("echo_rect_active.png");
		}
		else
		{
			echoRectSurface = IMG_Load("echo_rect_inactive.png");
		}

		SDL_Rect echoRectLocation1 = { 160, 75, 0, 0 };
		SDL_BlitSurface(echoRectSurface, NULL, guiSurface_, &echoRectLocation1);
		SDL_FreeSurface(echoRectSurface);

		if(echoStates_[1])
		{
			echoRectSurface = IMG_Load("echo_rect_active.png");
		}
		else
		{
			echoRectSurface = IMG_Load("echo_rect_inactive.png");
		}
		SDL_Rect echoRectLocation2 = { 160, 196, 0, 0 };
		SDL_BlitSurface(echoRectSurface, NULL, guiSurface_, &echoRectLocation2);
		SDL_FreeSurface(echoRectSurface);

		if(echoStates_[2])
		{
			echoRectSurface = IMG_Load("echo_rect_active.png");
		}
		else
		{
			echoRectSurface = IMG_Load("echo_rect_inactive.png");
		}
		SDL_Rect echoRectLocation3 = { 160, 317, 0, 0 };

		SDL_BlitSurface(echoRectSurface, NULL, guiSurface_, &echoRectLocation3);
		SDL_FreeSurface(echoRectSurface);

		SDL_Color foregroundColor = { 0, 0, 0 };

		SDL_Rect textLocationEcho = { 70, 195, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, echostr_.c_str(),
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationEcho);

		SDL_FreeSurface(textSurface_);

		SDL_Rect textLocationFast = { 572, 136, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, "Fast",
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationFast);

		SDL_FreeSurface(textSurface_);

		SDL_Rect textLocationMed = { 572, 257, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, "Med",
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationMed);

		SDL_FreeSurface(textSurface_);

		SDL_Rect textLocationSlow = { 572, 378, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, "Slow",
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationSlow);

		SDL_FreeSurface(textSurface_);
		}
		break;
	case eGuiFlange:
		{
		SDL_Surface *flangeLineSurface = IMG_Load("flange_depth_line.png");
		SDL_Rect flangeLineLocation = { 450, 125, 0, 0 };
		SDL_BlitSurface(flangeLineSurface, NULL, guiSurface_, &flangeLineLocation);
		SDL_FreeSurface(flangeLineSurface);

		SDL_Surface *flangeControlSurface = IMG_Load("control_button_active.png");
		SDL_Rect flangeControlLocation = { 438, flangeDepth_, 0, 0 };
		SDL_BlitSurface(flangeControlSurface, NULL, guiSurface_, &flangeControlLocation);
		SDL_FreeSurface(flangeControlSurface);

		SDL_Color foregroundColor = { 0, 0, 0 };
		SDL_Rect textLocationFlange = { 70, 295, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, flangestr_.c_str(),
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationFlange);

		SDL_FreeSurface(textSurface_);

		SDL_Rect textLocationDepth = { 425, 110, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, "Depth",
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationDepth);

		SDL_FreeSurface(textSurface_);
		}
		break;
	case eGuiTremolo:
		{
		SDL_Surface *tremoloLineSurface = IMG_Load("flange_depth_line.png");
		SDL_Rect tremoloLineLocation = { 450, 125, 0, 0 };
		SDL_BlitSurface(tremoloLineSurface, NULL, guiSurface_, &tremoloLineLocation);
		SDL_FreeSurface(tremoloLineSurface);

		SDL_Surface *tremoloControlSurface = IMG_Load("control_button_active.png");
		SDL_Rect tremoloControlLocation = { 438, tremoloRate_, 0, 0 };
		SDL_BlitSurface(tremoloControlSurface, NULL, guiSurface_, &tremoloControlLocation);
		SDL_FreeSurface(tremoloControlSurface);

		SDL_Color foregroundColor = { 0, 0, 0 };
		SDL_Rect textLocationTremolo = { 110, 395, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, tremolostr_.c_str(),
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationTremolo);

		SDL_FreeSurface(textSurface_);

		SDL_Rect textLocationRate = { 428, 110, 0, 0 };
		textSurface_ = TTF_RenderText_Blended(font2_, "Rate",
			foregroundColor);
		SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationRate);

		SDL_FreeSurface(textSurface_);
		}
		break;
	}

	SDL_Color foregroundColor = { 0, 0, 0 };
	textSurface_ = TTF_RenderText_Blended(font_, tempo_.c_str(),
		foregroundColor);

	SDL_Rect textLocation = { GUI_STRING_TEMPO_POS_X, GUI_STRING_TEMPO_POS_Y, 0, 0 };

	SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation);
	SDL_FreeSurface(textSurface_);

	SDL_Surface *timelineSurface = IMG_Load("timeline.png");
	SDL_Rect timelineLocation = { 35, 540, 0, 0 };
	SDL_BlitSurface(timelineSurface, NULL, guiSurface_, &timelineLocation);
	SDL_FreeSurface(timelineSurface);

	if(loop_.effect != 5)
	{
		SDL_Surface *loopSurface = IMG_Load("loop.png");

		SDL_Rect loopStartLocation = { 35+(timelineFactor_ * (float)loop_.start), 540, 0, 0 };
		SDL_Rect loopEndLocation = { 35+(timelineFactor_ * (float)loop_.end), 540, 0, 0 };

		SDL_BlitSurface(loopSurface, NULL, guiSurface_, &loopStartLocation);
		SDL_BlitSurface(loopSurface, NULL, guiSurface_, &loopEndLocation);

		SDL_FreeSurface(loopSurface);

		switch(loop_.effect) {
		case 0:
			{
				SDL_Color foregroundColor = { 0, 0, 0 };
				char str[20];

				sprintf(str,"EFFECT: Lowpass");
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation1 = { 400, 475, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation1);

				sprintf(str,"THRESHOLD: %.fHz",loop_.threshold);
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation2 = { 400, 500, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation2);
				SDL_FreeSurface(textSurface_);
			}
			break;
		case 1:
			{
				SDL_Color foregroundColor = { 0, 0, 0 };
				char str[20];

				sprintf(str,"EFFECT: Highpass");
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation1 = { 400, 475, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation1);

				sprintf(str,"THRESHOLD: %.fHz",loop_.threshold);
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation2 = { 400, 500, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation2);
				SDL_FreeSurface(textSurface_);
			}
			break;
		case 2:
			{
				SDL_Color foregroundColor = { 0, 0, 0 };
				char str[20];

				sprintf(str,"EFFECT: Echo");
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation1 = { 400, 475, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation1);

				sprintf(str,"THRESHOLD: %.f",loop_.threshold);
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation2 = { 400, 500, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation2);
				SDL_FreeSurface(textSurface_);
			}
			break;
		case 3:
			{
				SDL_Color foregroundColor = { 0, 0, 0 };
				char str[20];

				sprintf(str,"EFFECT: Flange");
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation1 = { 400, 475, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation1);

				sprintf(str,"THRESHOLD: %.f",loop_.threshold);
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation2 = { 400, 500, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation2);
				SDL_FreeSurface(textSurface_);
			}
			break;
		case 4:
			{
				SDL_Color foregroundColor = { 0, 0, 0 };
				char str[20];

				sprintf(str,"EFFECT: Tremolo");
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation1 = { 400, 475, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation1);

				sprintf(str,"THRESHOLD: %.fHz",loop_.threshold);
				textSurface_ = TTF_RenderText_Blended(font2_, str,
					foregroundColor);

				SDL_Rect textLocation2 = { 400, 500, 0, 0 };

				SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocation2);
				SDL_FreeSurface(textSurface_);
			}
			break;
		}

	}

	SDL_Surface *cursorSurface = IMG_Load("cursor.png");
	SDL_Rect cursorLocation = { currentTime_, 533, 0, 0 };
	SDL_BlitSurface(cursorSurface, NULL, guiSurface_, &cursorLocation);
	SDL_FreeSurface(cursorSurface);

	SDL_Rect textLocationTime = { 280, 560, 0, 0 };
	char curTime[20];

	sprintf(curTime, "%02d:%02d:%02d", curTimeMs_/1000/60, curTimeMs_/1000%60, curTimeMs_/10%100 );
	textSurface_ = TTF_RenderText_Blended(font2_, curTime,
		foregroundColor);

	SDL_BlitSurface(textSurface_, NULL, guiSurface_, &textLocationTime);
	SDL_FreeSurface(textSurface_);

	SDL_Surface *counterBarSurface = IMG_Load("counter_bar.png");
	SDL_Rect counterBarLocation = { 35, 15, 0, 0 };
	SDL_BlitSurface(counterBarSurface, NULL, guiSurface_, &counterBarLocation);
	SDL_FreeSurface(counterBarSurface);

	SDL_Surface *counterLineSurface = IMG_Load("counter_line.png");
	for(int i=0; i<beatCount_; i++)
	{
		SDL_Rect counterLineLocation = { 36+(i*26), 16, 0, 0 };
		SDL_BlitSurface(counterLineSurface, NULL, guiSurface_, &counterLineLocation);
	}
	SDL_FreeSurface(counterLineSurface);
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

void Gui::setEchoState(int index, bool state)
{
	echoStates_[index] = state;
}

bool Gui::checkEchoStates()
{
	for(int i=0; i<3; i++)
	{
		if(echoStates_[i])
		{
			return true;
		}
	}

	return false;
}

void Gui::setFlangeDepth(long &depth)
{
	flangeDepth_ = depth + GUI_SCREEN_OFFSET_Y;
}

void Gui::setTremoloRate(long &rate)
{
	tremoloRate_ = rate + GUI_SCREEN_OFFSET_Y;
}

void Gui::setTimelineLength(unsigned int length)
{
	timelineFactor_ = 570.0 / (float)length;
}

void Gui::setCurrentTime(unsigned int time)
{
	curTimeMs_ = time;
	currentTime_ = (timelineFactor_ * (float)time) + 28;
	if(currentTime_ > 605)
	{
		currentTime_ = 605;
	}
}

void Gui::setLoop(const Loop &loop)
{
	loop_ = loop;
}

void Gui::incBeatCount()
{
	beatCount_++;
}

void Gui::zeroBeatCount()
{
	beatCount_ = 1;
}

