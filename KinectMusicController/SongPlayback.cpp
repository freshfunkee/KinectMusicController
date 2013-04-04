#include "SongPlayback.h"

SongPlayback::SongPlayback(Song *song)
{
	path_ = song->getPath();
	mspb = 1000/(song->getBpm()/60);

	streamState_ = eSteamUninitialised;
}

SongPlayback::~SongPlayback()
{
	result_ = stream1_->release();
	errchk(result_);
	result_ = system_->close();
	errchk(result_);
	result_ = system_->release();
	errchk(result_);
}

int SongPlayback::initialize()
{
	result_ = FMOD::System_Create(&system_);
	errchk(result_);

	result_ = system_->getVersion(&version_);
	errchk(result_);

	if(version_ < FMOD_VERSION)
	{
		printf("Error! You are using an old version of FMOD %08x. Required: %08x.", version_, FMOD_VERSION);
		return 1;
	}

	result_ = system_->init(1, FMOD_INIT_NORMAL, 0);
	errchk(result_);

	result_ = system_->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pitch_);
	errchk(result_);

	result_ = system_->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &lowpass_);
	errchk(result_);

	result_ = system_->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &highpass_);
	errchk(result_);

	result_ = system_->createDSPByType(FMOD_DSP_TYPE_ECHO, &echo_);
	errchk(result_);

	result_ = system_->createDSPByType(FMOD_DSP_TYPE_FLANGE, &flange_);
	errchk(result_);

	result_ = system_->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &tremolo_);
	errchk(result_);

	result_ = system_->createStream(path_.c_str(), FMOD_HARDWARE | FMOD_LOOP_NORMAL| FMOD_2D, 0, &stream1_);
	errchk(result_);

	result_ = stream1_->getLength(&songLength_, FMOD_TIMEUNIT_MS);
	errchk(result_);

	return 0;
}

void SongPlayback::generateEcho(int echoType)
{
	switch(echoType) {
	case 0:
		echo_->setParameter(FMOD_DSP_ECHO_DECAYRATIO, 0.7);
		echo_->setParameter(FMOD_DSP_ECHO_DELAY, 300);
		
		Sleep(350);
		break;
	case 1:
		echo_->setParameter(FMOD_DSP_ECHO_DECAYRATIO, 0.5);
		echo_->setParameter(FMOD_DSP_ECHO_DELAY, 500);
		
		Sleep(500);
		break;
	case 2:
		echo_->setParameter(FMOD_DSP_ECHO_DECAYRATIO, 0.3);
		echo_->setParameter(FMOD_DSP_ECHO_DELAY, 800);
		
		Sleep(850);
		break;
	}

	echo_->setParameter(FMOD_DSP_ECHO_WETMIX, 1);
	echo_->setParameter(FMOD_DSP_ECHO_DRYMIX, 0);

	channel_->setVolume(0);

	Sleep(2000);

	echo_->setParameter(FMOD_DSP_ECHO_WETMIX, 0);
	echo_->setParameter(FMOD_DSP_ECHO_DRYMIX, 1);

	for(float i=0.0;i<=1;i+=0.05)
	{
		channel_->setVolume(i);
		Sleep(50);
	}
}

void SongPlayback::getCurrentTime(unsigned int &time)
{
	result_ = channel_->getPosition(&time, FMOD_TIMEUNIT_MS);
	errchk(result_);
}

void SongPlayback::setPlaybackRate(unsigned int& sum)
{
	spfreq_ = defreq_ + (defreq_ * (float)((mspb - (float)sum)/mspb));

	result_ = channel_->setFrequency(spfreq_);
	errchk(result_);

	if((defreq_/spfreq_)>0)
	{
		result_ = pitch_->setParameter(FMOD_DSP_PITCHSHIFT_PITCH, (defreq_/spfreq_));
		errchk(result_);
	}

	result_ = system_->update();
	errchk(result_);

	//printf("Current Freq: %f\n", spfreq_);
}

void SongPlayback::setLowpassCutoff(float &cutoff)
{
	result_ = lowpass_->setParameter(FMOD_DSP_LOWPASS_CUTOFF, cutoff);
	errchk(result_);

	result_ = system_->update();
	errchk(result_);
}

void SongPlayback::setHighpassCutoff(float &cutoff)
{
	result_ = highpass_->setParameter(FMOD_DSP_HIGHPASS_CUTOFF, cutoff);
	errchk(result_);

	result_ = system_->update();
	errchk(result_);
}

void SongPlayback::setFlangeDepth(float &depth)
{
	result_ = flange_->setParameter(FMOD_DSP_FLANGE_DEPTH, depth);
	errchk(result_);

	result_ = system_->update();
	errchk(result_);
}

void SongPlayback::setTremoloRate(float &rate)
{
	result_ = tremolo_->setParameter(FMOD_DSP_TREMOLO_FREQUENCY, rate);
	errchk(result_);

	result_ = system_->update();
	errchk(result_);
}

void SongPlayback::setFlangeActive(bool active)
{
	if(active)
	{
		result_ = flange_->setParameter(FMOD_DSP_FLANGE_WETMIX, 0.55);
		errchk(result_);

		result_ = flange_->setParameter(FMOD_DSP_FLANGE_DRYMIX, 0.45);
		errchk(result_);
	}
	else
	{
		result_ = flange_->setParameter(FMOD_DSP_FLANGE_WETMIX, 0);
		errchk(result_);

		result_ = flange_->setParameter(FMOD_DSP_FLANGE_DRYMIX, 1);
		errchk(result_);
	}

	result_ = system_->update();
	errchk(result_);
}

void SongPlayback::setTremoloActive(bool active)
{
	
	if(active)
	{
		result_ = tremolo_->setParameter(FMOD_DSP_TREMOLO_DEPTH, 0.7);
		errchk(result_);
	}
	else
	{
		result_ = tremolo_->setParameter(FMOD_DSP_TREMOLO_DEPTH, 0);
		errchk(result_);

		result_ = tremolo_->setParameter(FMOD_DSP_TREMOLO_FREQUENCY, 0);
		errchk(result_);
	}

	result_ = system_->update();
	errchk(result_);
}

void SongPlayback::startPlayback()
{
	result_ = system_->playSound(FMOD_CHANNEL_FREE, stream1_, false, &channel_);
	errchk(result_);

	result_ = channel_->getFrequency(&defreq_);
	errchk(result_);

	printf("\n Default freq is: %.2f\n\n", defreq_);

	result_ = pitch_->setParameter(FMOD_DSP_PITCHSHIFT_FFTSIZE, 4096);
	errchk(result_);

	result_ = lowpass_->setParameter(FMOD_DSP_LOWPASS_CUTOFF, 20000.0);
	errchk(result_);

	result_ = highpass_->setParameter(FMOD_DSP_HIGHPASS_CUTOFF, 20.0);
	errchk(result_);

	result_ = flange_->setParameter(FMOD_DSP_FLANGE_RATE, 2);
	errchk(result_);
		
	result_ = echo_->setParameter(FMOD_DSP_ECHO_DRYMIX, 1);
	errchk(result_);

	result_ = echo_->setParameter(FMOD_DSP_ECHO_WETMIX, 0);
	errchk(result_);

	result_ = system_->addDSP(pitch_,0);
	errchk(result_);

	result_ = system_->addDSP(lowpass_,0);
	errchk(result_);

	result_ = system_->addDSP(highpass_,0);
	errchk(result_);

	result_ = system_->addDSP(flange_,0);
	errchk(result_);

	result_ = system_->addDSP(tremolo_,0);
	errchk(result_);

	result_ = system_->addDSP(echo_,0);
	errchk(result_);

	result_ = pitch_->setActive(active_);
	errchk(result_);

	result_ = lowpass_->setActive(active_);
	errchk(result_);

	setFlangeActive(false);
	setTremoloActive(false);

	streamState_ = eStreamPlaying;
}

void SongPlayback::pausePlayback()
{
	result_ = channel_->setPaused(true);
	errchk(result_);

	streamState_ = eStreamPaused;
}

void SongPlayback::resumePlayback()
{
	result_ = channel_->setPaused(false);
	errchk(result_);

	streamState_ = eStreamPlaying;
}