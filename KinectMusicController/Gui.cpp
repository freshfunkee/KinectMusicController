#include "Gui.h"

Gui::Gui()
{
	screen_ = NULL;
	surface_ = NULL;
}

Gui::~Gui()
{
	SDL_FreeSurface(pauseSurface_);
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

	pauseSurface_ = IMG_Load("PauseButton.png");
	jointImg_ = IMG_Load("Joint.png");
	font_ = TTF_OpenFont("arial.ttf", 12);
}

/*void Gui::drawTempoString( std::string &tempo )
{
	if(!font_)
	{
		printf("\n\nfont fialed to load");
	}
	SDL_Color foregroundColor = { 255, 255, 255 };
	SDL_Color backgroundColor = { 0, 0, 255 };
	textSurface_ = TTF_RenderText_Shaded(font_, tempo.c_str(),
		foregroundColor, backgroundColor);

	SDL_Rect textLocation = { 25, 400, 0, 0 };

	SDL_BlitSurface(textSurface_, NULL, screen_, &textLocation);
	SDL_FreeSurface(textSurface_);
}
*/

void Gui::displayFrame(long **skelMatrix)
{
	if (!screen_) 
    {
        screen_ = SDL_SetVideoMode(640, 680, 0, SDL_HWSURFACE | SDL_NOFRAME);
        if (!screen_) 
        {
            fprintf(stderr, "SDL: could not set video mode - exiting\n");
            exit(1);
        }
    }

	surface_ = SDL_CreateRGBSurface(SDL_HWSURFACE, 640, 480, 16, 0, 0, 0, 0);
	SDL_FillRect( surface_, NULL, 248);
	
	for(int i=0;i<20;i++)
	{
		SDL_Rect posRect = { skelMatrix[i][0], skelMatrix[i][1], 0, 0 };
		SDL_BlitSurface( jointImg_, 0, surface_, &posRect );
	}

	SDL_Rect mainScreen = { 0, 100, 0, 0 };
	if(SDL_BlitSurface(surface_, 0, screen_, &mainScreen)==-1)
	{
		printf("SDL Error! %s ",SDL_GetError());
		if(surface_ == NULL)
			printf("surface_ is NULL\n");

		if(screen_ == NULL)
			printf("screen_ is NULL\n");
	}

	SDL_FreeSurface(surface_);
	drawGui();

    SDL_Flip(screen_);
}

void Gui::drawGui()
{
	SDL_Rect pauseLocation = { 50, 25, 0, 0 };
	SDL_BlitSurface(pauseSurface_, NULL, screen_, &pauseLocation);
}


