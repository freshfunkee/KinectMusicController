#ifndef SONGPLAYBACK_H
#define SONGPLAYBACK_H

#include <stdio.h>
#include <string>
#include <cstdlib>

#include <fmod\fmod.hpp>
#include <fmod\fmod_errors.h>

#include "Song.h"

typedef enum {
	eSteamUninitialised,
	eStreamPaused,
	eStreamPlaying
} StreamState;

class SongPlayback
{
public:
	SongPlayback(Song*);
	~SongPlayback();

	int initialize();
	void setPlaybackRate(int&);
	void setLowpassCutoff(float&);
	void startPlayback();
	void pausePlayback();
	void resumePlayback();
	void setPlaybackState(const StreamState&);
	StreamState getPlaybackState() { return streamState_; }

private:
	FMOD::System *system_;
	FMOD::Channel *channel_;
	FMOD::Sound *stream1_;
	FMOD::DSP *pitch_, *lowpass_;

	unsigned int version_;
	float defreq_, spfreq_, pit_;
	bool active_;
	string path_;
	float mspb;

	StreamState streamState_;
	FMOD_RESULT result_;

	void errchk(FMOD_RESULT r) 
	{
		if(r != FMOD_OK)						
		{										
			printf("FMOD_ERROR! (%d) %s\n", r, FMOD_ErrorString(r));	
		}
	}
};

#endif