#include "HandMonitor.h"

#include "Gui.h"

#define HOVER_TIME 2000

HandMonitor::HandMonitor()
{
}

HandMonitor::~HandMonitor()
{
}

void HandMonitor::monitor()
{
	while(!done)
	{
		//getpositions
		//check if over a button
	}
}

bool HandMonitor::checkTimeout(Uint32 &startTime)
{
	if(startTime == 0)
		startTime = SDL_GetTicks();
	else if((SDL_GetTicks() - startTime) > HOVER_TIME)
		return true;

	return false;
}