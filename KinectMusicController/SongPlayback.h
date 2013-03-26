#ifndef SONGPLAYBACK_H
#define SONGPLAYBACK_H

#include <stdio.h>
#include <string>
#include <cstdlib>
#include <Windows.h>

#include <fmod\fmod.hpp>
#include <fmod\fmod_errors.h>

#include "Song.h"

typedef enum {
	eSteamUninitialised,
	eStreamPaused,
	eStreamPlaying,
} StreamState;

class SongPlayback
{
public:
	SongPlayback(Song*);
	~SongPlayback();

	int initialize();

	void generateEcho(int);

	void setPlaybackRate(unsigned int&);
	void setLowpassCutoff(float&);
	void setHighpassCutoff(float&);
	void setFlangeActive(bool);
	void setTremoloActive(bool);
	void setFlangeDepth(float&);
	void setTremoloRate(float&);

	void startPlayback();
	void pausePlayback();
	void resumePlayback();
	void setPlaybackState(const StreamState&);
	StreamState getPlaybackState() { return streamState_; }

	static void EchoShortThreadEntry(void *pThis)
	{
		SongPlayback *playback = (SongPlayback*) pThis;

		playback->generateEcho(0);
	}

	static void EchoMedThreadEntry(void *pThis)
	{
		SongPlayback *playback = (SongPlayback*) pThis;

		playback->generateEcho(1);
	}

	static void EchoLongThreadEntry(void *pThis)
	{
		SongPlayback *playback = (SongPlayback*) pThis;

		playback->generateEcho(2);
	}

private:
	FMOD::System *system_;
	FMOD::Channel *channel_;
	FMOD::Sound *stream1_;
	FMOD::DSP *pitch_, *lowpass_, *highpass_, *echo_, *flange_, *tremolo_;

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