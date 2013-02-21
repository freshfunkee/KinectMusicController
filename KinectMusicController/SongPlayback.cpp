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

	result_ = system_->createStream(path_.c_str(), FMOD_HARDWARE | FMOD_LOOP_NORMAL| FMOD_2D, 0, &stream1_);
	errchk(result_);

	return 0;
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

	result_ = system_->addDSP(pitch_,0);
	errchk(result_);

	result_ = system_->addDSP(lowpass_,0);
	errchk(result_);

	result_ = system_->addDSP(highpass_,0);
	errchk(result_);

	result_ = pitch_->setActive(active_);
	errchk(result_);

	result_ = lowpass_->setActive(active_);
	errchk(result_);

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