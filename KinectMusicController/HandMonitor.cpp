#include "HandMonitor.h"

#include <sstream>
#include <cmath>
#include <process.h>

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
	startHover_ = new unsigned int[5];


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
		startHover_[i] = 0;
	}
}

HandMonitor::~HandMonitor()
{
	delete [] measures_;
	delete [] buttonStates_;
	delete [] startHover_;
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
			if(checkTimeout(startHover_[0],GUI_BUTTON_PAUSE_INDEX))
			{
				playback_->resumePlayback();
			}
		}
		else
		{
			startHover_[0] = 0;
			buttonStates_[GUI_BUTTON_PAUSE_INDEX] = eButtonActive;
		}
		break;
	case eSteamUninitialised:
		if( lhY_ > (GUI_BUTTON_PAUSE_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_PAUSE_POS_Y + GUI_BUTTON_SIZE) - GUI_SCREEN_OFFSET_Y) && 
				lhX_ > GUI_BUTTON_PAUSE_POS_X && 
				lhX_ < (GUI_BUTTON_PAUSE_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[0],GUI_BUTTON_PAUSE_INDEX))
			{
				playback_->startPlayback();
			}
		}
		else
		{
			startHover_[0] = 0;
			buttonStates_[GUI_BUTTON_PAUSE_INDEX] = eButtonActive;
		}
		break;
	case eStreamPlaying:
		if( lhY_ > (GUI_BUTTON_PAUSE_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_PAUSE_POS_Y + GUI_BUTTON_SIZE) - GUI_SCREEN_OFFSET_Y) && 
				lhX_ > GUI_BUTTON_PAUSE_POS_X && 
				lhX_ < (GUI_BUTTON_PAUSE_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[0],GUI_BUTTON_PAUSE_INDEX))
			{
				playback_->pausePlayback();
			}
		}
		else
		{
			startHover_[0] = 0;
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
			if(checkTimeout(startHover_[1],GUI_BUTTON_FILTER_INDEX))
			{
				gui_->displayFilter();
			}
		}
		else if( lhY_ > (GUI_BUTTON_2_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_2_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_2_POS_X && 
				lhX_ < (GUI_BUTTON_2_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[2],GUI_BUTTON_2_INDEX))
			{
				gui_->displayEcho();
			}
		}
		else if( lhY_ > (GUI_BUTTON_3_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_3_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_3_POS_X && 
				lhX_ < (GUI_BUTTON_3_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[3],GUI_BUTTON_3_INDEX))
			{
				playback_->setFlangeActive(true);
				gui_->displayFlange();
			}
		}
		else if( lhY_ > (GUI_BUTTON_4_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_4_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_4_POS_X && 
				lhX_ < (GUI_BUTTON_4_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[4],GUI_BUTTON_4_INDEX))
			{
				playback_->setTremoloActive(true);
				gui_->displayTremolo();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_FILTER_INDEX] = eButtonIdle;
			buttonStates_[GUI_BUTTON_2_INDEX] = eButtonIdle;
			buttonStates_[GUI_BUTTON_3_INDEX] = eButtonIdle;
			buttonStates_[GUI_BUTTON_4_INDEX] = eButtonIdle;
			startHover_[1] = 0;
			startHover_[2] = 0;
		}

		setTempoParams();
		break;
	case eGuiFilter:
		if( lhY_ > (GUI_BUTTON_FILTER_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_FILTER_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_FILTER_POS_X && 
				lhX_ < (GUI_BUTTON_FILTER_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[1],GUI_BUTTON_FILTER_INDEX))
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
			startHover_[1] = 0;
		}

		setFilterParams();
		break;
	case eGuiEcho:
		if( lhY_ > (GUI_BUTTON_2_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_2_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_2_POS_X && 
				lhX_ < (GUI_BUTTON_2_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[2],GUI_BUTTON_2_INDEX))
			{
				gui_->displayTempo();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_FILTER_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_2_INDEX] = eButtonActive;
			buttonStates_[GUI_BUTTON_3_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_4_INDEX] = eButtonInactive;
			startHover_[2] = 0;
		}

		setEchoParams();
		break;
	case eGuiFlange:
		if( lhY_ > (GUI_BUTTON_3_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_3_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_3_POS_X && 
				lhX_ < (GUI_BUTTON_3_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[3],GUI_BUTTON_3_INDEX))
			{
				playback_->setFlangeActive(false);
				gui_->displayTempo();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_FILTER_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_2_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_3_INDEX] = eButtonActive;
			buttonStates_[GUI_BUTTON_4_INDEX] = eButtonInactive;
			startHover_[3] = 0;
		}

		setFlangeParams();
		break;
	case eGuiTremolo:
		if( lhY_ > (GUI_BUTTON_4_POS_Y - GUI_SCREEN_OFFSET_Y) && 
				lhY_ < ((GUI_BUTTON_4_POS_Y - GUI_SCREEN_OFFSET_Y) + GUI_BUTTON_SIZE) && 
				lhX_ > GUI_BUTTON_4_POS_X && 
				lhX_ < (GUI_BUTTON_4_POS_X + GUI_BUTTON_SIZE) )
		{
			if(checkTimeout(startHover_[4],GUI_BUTTON_4_INDEX))
			{
				playback_->setTremoloActive(false);
				gui_->displayTempo();
			}
		}
		else
		{
			buttonStates_[GUI_BUTTON_FILTER_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_2_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_3_INDEX] = eButtonInactive;
			buttonStates_[GUI_BUTTON_4_INDEX] = eButtonActive;
			startHover_[4] = 0;
		}
		setTremoloParams();
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

void HandMonitor::setEchoParams()
{
	if(lhY_ > 25 && lhY_ < 145 && rhY_ > 25 && rhY_ < 145)
	{
		if(rhX_ - lhX_ < 20)
		{
			if(!(gui_->checkEchoStates()))
			{
				_beginthread( SongPlayback::EchoShortThreadEntry, 0, (void*)playback_ );
				gui_->setEchoState(0,true);
			}
		}
	}
	else if(lhY_ > 146 && lhY_ < 267 && rhY_ > 146 && rhY_ < 267)
	{
		if(rhX_ - lhX_ < 20)
		{
			if(!(gui_->checkEchoStates()))
			{
				_beginthread( SongPlayback::EchoMedThreadEntry, 0, (void*)playback_ );
				gui_->setEchoState(1,true);
			}
		}
	}
	else if(lhY_ > 268 && lhY_ < 389 && rhY_ > 268 && rhY_ < 389)
	{
		if(rhX_ - lhX_ < 20)
		{
			if(!(gui_->checkEchoStates()))
			{
				_beginthread( SongPlayback::EchoLongThreadEntry, 0, (void*)playback_ );
				gui_->setEchoState(2,true);
			}
		}
	}

	if(rhX_ - lhX_ > 100)
	{
		gui_->setEchoState(0,false);
		gui_->setEchoState(1,false);
		gui_->setEchoState(2,false);
	}
}

void HandMonitor::setFlangeParams()
{
	if(rhY_ > 75 && rhY_ < 352)
	{
		float depth = (277-(((float)rhY_ - 75)*0.0036))/277;

		playback_->setFlangeDepth(depth);
		gui_->setFlangeDepth(rhY_);
	}
	else if(rhY_ < 75)
	{
		long pos = 75;
		float depth = 0;

		playback_->setFlangeDepth(depth);
		gui_->setFlangeDepth(pos);
	}
	else if(rhY_ > 352)
	{
		long pos = 352;
		float depth = 1;

		playback_->setFlangeDepth(depth);
		gui_->setFlangeDepth(pos);
	}
}

void HandMonitor::setTremoloParams()
{
	if(rhY_ > 75 && rhY_ < 352)
	{
		gui_->setTremoloRate(rhY_);
		//TODO calculate graphics to sound engine conversion
	}
	else if(rhY_ < 75)
	{
		long pos = 75;
		float rate = 0.1;

		playback_->setTremoloRate(rate);
		gui_->setTremoloRate(pos);
	}
	else if(rhY_ > 352)
	{
		long pos = 352;
		float rate = 10;

		playback_->setTremoloRate(rate);
		gui_->setTremoloRate(pos);
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
