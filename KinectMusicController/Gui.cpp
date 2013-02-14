#include "Gui.h"
#include "CVImage.h"

Gui::Gui(CVImage *image)
{
	cvImage_ = image;
	screen_ = NULL;
	surface_ = NULL;
	pauseSurface_ = IMG_Load("PauseButton.png");
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
	printf("SDL Closed");
}

void Gui::initialize()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	font_ = TTF_OpenFont("arial.ttf", 12);
}

//void Gui::drawSurfaceFromMatrix(IplImage &img)
//{
//	if (!screen_) 
//    {
//        screen_ = SDL_SetVideoMode(img.width, img.height, 0, 0);
//        if (!screen_) 
//        {
//            fprintf(stderr, "SDL: could not set video mode - exiting\n");
//            exit(1);
//        }
//    }
//
//    // Assuming IplImage packed as BGR 24bits
//    surface_ = SDL_CreateRGBSurfaceFrom((void*)img.imageData,
//                img.width,
//                img.height,
//                img.depth * img.nChannels,
//                img.widthStep,
//                0xff0000, 0x00ff00, 0x0000ff, 0
//                );
//
//    if(SDL_BlitSurface(surface_, 0, screen_, 0)==-1)
//	{
//		printf("SDL Error! %s ",SDL_GetError());
//		if(surface_ == NULL)
//			printf("surface_ is NULL\n");
//
//		if(screen_ == NULL)
//			printf("screen_ is NULL\n");
//	}
//
//	SDL_FreeSurface(surface_);
//}

void Gui::drawTempoString( std::string &tempo )
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

void Gui::displayFrame()
{
	cvImage_->getPixelMap( frame_ );

	if (!screen_) 
    {
        screen_ = SDL_SetVideoMode(1280, 960, 0, SDL_SWSURFACE | SDL_NOFRAME);
        if (!screen_) 
        {
            fprintf(stderr, "SDL: could not set video mode - exiting\n");
            exit(1);
        }
    }

	surface_ = SDL_CreateRGBSurfaceFrom((void*)frame_.imageData,
                frame_.width,
                frame_.height,
                frame_.depth * frame_.nChannels,
                frame_.widthStep,
                0xff0000, 0x00ff00, 0x0000ff, 0
                );

	if(SDL_BlitSurface(surface_, 0, screen_, 0)==-1)
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


