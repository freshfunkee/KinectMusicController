#ifndef HANDMONITOR_H
#define HANDMONITOR_H

#include "Gui.h"
class SongPlayback;

class HandMonitor
{
public:
	HandMonitor(SongPlayback*,Gui*);
	~HandMonitor();

	void init();
	void monitor(long **);
private:
	bool checkTimeout(unsigned int &, int button);
	void checkStates();
	void setTempoParams();
	void setFilterParams();
	void setEchoParams();
	void setFlangeParams();
	void setTremoloParams();
	void addTempoMeasure();
	void calcTempo();

	long rhX_,rhY_,rhYprev_,rhYrel_,rhZ_,lhX_,lhY_,lhZ_, initialDepth_;
	unsigned int *startHover_, curTime_, prevTime_, sumMeasurePrev_, *measures_;
	short measureCount_;
	bool decY_;
	
	ButtonState *buttonStates_;
	SongPlayback *playback_;
	Gui *gui_;
};
#endif