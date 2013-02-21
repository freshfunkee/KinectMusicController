#include "HandMonitor.h"

#include <sstream>
#include <cmath>

#include "Gui.h"
#include "SongPlayback.h"

#define HOVER_TIME 2000

#define KINECT_HAND_LEFT 7
#define KINECT_HAND_RIGHT 11

#define X_POS 0
#define Y_POS 1
#define Z_POS 2

#define TEMPO_SAMPLES 8
#define TEMPO_THRESHOLD 25

HandMonitor::HandMonitor(SongPlayback *playback, Gui *gui)
{
	playback_ = playback;
	gui_ = gui;

	rhX_ = 0;
	rhY_ = 0;
	rhYprev_ = 0;
	rhYrel_ = 0;
	rhZ_ = 0;

	lhX_ = 0;
	lhY_ = 0;
	lhZ_ = 0;

	initialDepth_ = 0;

	curTime_ = 0;
	prevTime_ = 0; 
	sumMeasurePrev_ = 0;
	startHover_ = 0;

	measures_ = new unsigned int[TEMPO_SAMPLES];
	for(int i=0;i<TEMPO_SAMPLES;i++)
	{
		measures_[i] = 0;
	}

	measureCount_ = 0;
	decY_ = false;

	buttonStates_ = new ButtonState[5];
	for(int i=0;i<5;i++)
	{
		buttonStates_[i] = eButtonIdle;
	}
}

HandMonitor::~HandMonitor()
{
	delete [] measures_;
	delete [] buttonStates_;
}

void HandMonitor::monitor(long **skelMatrix)
{
	rhX_ = skelMatrix[KINECT_HAND_RIGHT][X_POS];
	rhY_ = skelMatrix[KINECT_HAND_RIGHT][Y_POS];
	rhZ_ = skelMatrix[KINECT_HAND_RIGHT][Z_POS];

	lhX_ = skelMatrix[KINECT_HAND_LEFT][X_POS];
	lhY_ = skelMatrix[KINECT_HAND_LEFT][Y_POS];
	lhZ_ = skelMatrix[KINECT_HAND_LEFT][Z_POS];

	if(initialDepth_ == 0 && lhZ_ > 0)
	{
		initialDepth_ = lhZ_;
	}

	checkStates();

	gui_->setButtonStates(buttonStates_);
	gui_->displayFrame(skelMatrix);
}

void HandMonitor::checkStates()
{
	switch(playback_->getPlaybackState()) 
	{
	case eStreamPaused:
		if( lhY_ > (GUI_BUTTON_PAUSE_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_PAUSE_POS_Y + GUI_BUTTON_SIZE) - GUI_SCREEN_OFFSET_Y) && 
				lhX_ > GUI_BUTTON_PAUSE_POS_X && 
				lhX_ < (GUI_BUTTON_PAUSE_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_,GUI_BUTTON_PAUSE_INDEX))
			{
				playback_->resumePlayback();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_PAUSE_INDEX] = eButtonActive;
		}
		break;
	case eSteamUninitialised:
		if( lhY_ > (GUI_BUTTON_PAUSE_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_PAUSE_POS_Y + GUI_BUTTON_SIZE) - GUI_SCREEN_OFFSET_Y) && 
				lhX_ > GUI_BUTTON_PAUSE_POS_X && 
				lhX_ < (GUI_BUTTON_PAUSE_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_,GUI_BUTTON_PAUSE_INDEX))
			{
				playback_->startPlayback();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_PAUSE_INDEX] = eButtonActive;
		}
		break;
	case eStreamPlaying:
		if( lhY_ > (GUI_BUTTON_PAUSE_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_PAUSE_POS_Y + GUI_BUTTON_SIZE) - GUI_SCREEN_OFFSET_Y) && 
				lhX_ > GUI_BUTTON_PAUSE_POS_X && 
				lhX_ < (GUI_BUTTON_PAUSE_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_,GUI_BUTTON_PAUSE_INDEX))
			{
				playback_->pausePlayback();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_PAUSE_INDEX] = eButtonIdle;
		}
		break;
	}

	switch(gui_->getGuiState())
	{
	case eGuiTempo:
		if( lhY_ > (GUI_BUTTON_FILTER_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_FILTER_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_FILTER_POS_X && 
				lhX_ < (GUI_BUTTON_FILTER_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_,GUI_BUTTON_FILTER_INDEX))
			{
				gui_->displayFilter();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_FILTER_INDEX] = eButtonIdle;
			buttonStates_[GUI_BUTTON_2_INDEX] = eButtonIdle;
			buttonStates_[GUI_BUTTON_3_INDEX] = eButtonIdle;
			buttonStates_[GUI_BUTTON_4_INDEX] = eButtonIdle;
		}

		setTempoParams();
		break;
	case eGuiFilter:
		if( lhY_ > (GUI_BUTTON_FILTER_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_FILTER_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_FILTER_POS_X && 
				lhX_ < (GUI_BUTTON_FILTER_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_,GUI_BUTTON_FILTER_INDEX))
			{
				gui_->displayTempo();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_FILTER_INDEX] = eButtonActive;
			buttonStates_[GUI_BUTTON_2_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_3_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_4_INDEX] = eButtonInactive;
		}

		setFilterParams();
		break;
	}
}

void HandMonitor::setTempoParams()
{
	switch(playback_->getPlaybackState())
	{
	case eStreamPlaying:
		rhYrel_ = rhY_ - rhYprev_;
		if(!decY_)
		{
			if(rhYrel_ < -1)
			{
				addTempoMeasure();
				if(measureCount_ >= TEMPO_SAMPLES)
				{
					calcTempo();
				}
			}
		}
		else
		{
			if(rhYrel_ > 1)
			{
				addTempoMeasure();
				if(measureCount_ >= TEMPO_SAMPLES)
				{
					calcTempo();
				}
			}
		}
		rhYprev_ = rhY_;
		break;
	}
}

void HandMonitor::addTempoMeasure()
{
	decY_ = !decY_;
	curTime_ = SDL_GetTicks();
	measures_[measureCount_] = curTime_ - prevTime_;
	prevTime_ = curTime_;
	measureCount_++;
}

void HandMonitor::calcTempo()
{
	unsigned int lowMeasure = 0, highMeasure = 0, sumMeasure =0;

	for(int i=0;i<measureCount_;i++)
	{
		if(measures_[i] > measures_[highMeasure])
		{
			highMeasure = i;
		}
		if(measures_[i] < measures_[lowMeasure])
		{
			lowMeasure = i;
		}
	}

	for(int i=0;i<measureCount_;i++)
	{
		if(i != highMeasure && i != lowMeasure)
		{
			sumMeasure += measures_[i];
		}
	}

	sumMeasure /= TEMPO_SAMPLES-2;

	if(((sumMeasure > sumMeasurePrev_+TEMPO_THRESHOLD) || (sumMeasure < sumMeasurePrev_-TEMPO_THRESHOLD)) && sumMeasure > 0)
	{
		playback_->setPlaybackRate(sumMeasure);
		float bpm = (1000/(float)sumMeasure) * 60;
		gui_->setTempoString((long)bpm);
	}

	measureCount_ = 0;
	sumMeasurePrev_ = sumMeasure;
}

void HandMonitor::setFilterParams()
{
	//TODO
	//Get gui values and send it to sound engine
	if((initialDepth_ - lhZ_) > 300)
	{
		if(lhX_ > 200 && lhX_ < 600)
		{
			gui_->setHighpass(lhX_);

			float powY = ((float)lhX_-200)/93;
			float cutoff = pow(10,powY);
			playback_->setHighpassCutoff(cutoff);
		}
		else if(lhX_ < 200)
		{
			long pos = 200;
			float cutoff = 10;
			gui_->setHighpass(pos);

			playback_->setHighpassCutoff(cutoff);
		}
		else if(lhX_ > 600)
		{
			long pos = 600;
			float cutoff = 20000;
			gui_->setHighpass(pos);

			playback_->setHighpassCutoff(cutoff);
		}
	}

	if((initialDepth_ - rhZ_) > 300)
	{
		if(rhX_ > 200 && rhX_ < 600)
		{
			gui_->setLowpass(rhX_);

			float powY = ((float)rhX_-200)/93;
			float cutoff = pow(10,powY);
			playback_->setLowpassCutoff(cutoff);
			printf("\r%.2f lowpass", cutoff);
		}
		else if(rhX_ < 200)
		{
			long pos = 200;
			float cutoff = 10;
			gui_->setLowpass(pos);

			playback_->setLowpassCutoff(cutoff);
			printf("\r%.2f lowpass", cutoff);
		}
		else if(rhX_ > 600)
		{
			long pos = 600;
			float cutoff = 20000;
			gui_->setLowpass(pos);

			playback_->setLowpassCutoff(cutoff);
			printf("\r%.2f lowpass", cutoff);
		}
	}
	
}


bool HandMonitor::checkTimeout(unsigned int &startTime, int button)
{
	if(startTime == 0)
	{
		startTime = SDL_GetTicks();
		buttonStates_[button] = eButtonHover;
	}
	else if((SDL_GetTicks() - startTime) > HOVER_TIME/2 && (SDL_GetTicks() - startTime) < HOVER_TIME)
	{
		buttonStates_[button] = eButtonHover1000;
	}
	else if((SDL_GetTicks() - startTime) > HOVER_TIME)
	{
		startTime = 0;
		return true;
	}

	return false;
}
