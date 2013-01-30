//#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include <conio.h>

/*
#ifdef _DEBUG   
#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
#endif
#endif  // _DEBUG
*/

#include <string>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "CVImage.h"
#include "KinectController.h"
#include "Gui.h"
#include "SongPlayback.h"

#define XML_PATH "..\\songs"
#define TEMPO_THRESHOLD 25
#define TIME_SAMPLES 8
#define HOVER_TIME 2000

using namespace std;
namespace fs = boost::filesystem;

string chooseFile();
bool checkPressTime(Uint32&);

int main()
{
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	printf("--Kinect Music Controller\n--Version: 0.3\n--Authour: Eoin Joyce\n--Features:"
				"\n\t*Real-time Tempo Control\n\t*Kinect User Inupt\n\t*Xml Parsing\n\t*Left Hand Tracking\n\t*Play Button");
	printf("\n\nWelcome! Select a song to play:");

	string path = chooseFile();

	Song *trackSelection = new Song();
	trackSelection->readSongXml(path.c_str());

	cout << "\nTrack:\t" << trackSelection->getTitle() << "\nArtist:\t" << trackSelection->getArtist() << endl;

	int now = 0, prev = 0, interval [TIME_SAMPLES] = {0}, previnterval = 0, min =0, max =0, sum=0;
	int count =0,i=0;
	bool decY = false;

	SongPlayback *playback = new SongPlayback(trackSelection);

	CVImage *cvImage = new CVImage(playback);
	KinectController *kinect = new KinectController(cvImage);
	Gui *gui = new Gui(cvImage);

	kinect->initialize();
	playback->initialize();
	gui->initialize();

	delete(trackSelection);

	float rhPos, lhPos, prevPos=0, y;
	cv::Mat pixels;
	printf("\n\n");

	StreamState state = eStreamPaused;
	Uint32 startHover = 0;
	string tempo = "Tempo";
	float bpm;

	while( !(_kbhit()) )
	{
		rhPos = kinect->getHandRightPos();
		lhPos = kinect->getHandLeftPos();

		kinect->getPixelMap();

		switch(playback->getPlaybackState()) 
		{
		case eStreamPaused:
			if( lhPos > 25 && lhPos < 75 )
			{
				if(checkPressTime(startHover))
				{
					playback->resumePlayback();
					startHover = 0;
				}
			}
			break;
		case eSteamUninitialised:
			if( lhPos > 25 && lhPos < 75 )
			{
				if(checkPressTime(startHover))
				{
					playback->startPlayback();
					startHover = 0;
				}
			}
			break;
		case eStreamPlaying:
			if( lhPos > 25 && lhPos < 75 )
			{
				if(checkPressTime(startHover))
				{
					playback->pausePlayback();
					startHover = 0;
				}
			}
			y = rhPos - prevPos;

			if(!decY)
			{
				if(y < -1)
				{
					decY = true;
					now = SDL_GetTicks();
					interval[count] = now - prev;
					prev = now;
					//printf("Direction changed to 1. Y is decreasing(upscreen), time interval %i\n", interval[count]);
					count++;

					if(count > TIME_SAMPLES-1)
					{
						for(i=0;i<count;i++)
						{
							if(i == 0)
							{
								min = 0;
								max = 0;
							} 
							else
							{
								if(interval[i] > interval[max])
								{
									max = count;
								}
								if(interval[i] < interval[min])
								{
									min = count;
								}
							}
						}

						for(i=0;i<count;i++)
						{
							if(i != max && i != min)
							{
								sum += interval[i];
							}
						}

						sum /= TIME_SAMPLES-2;

						if(((sum > previnterval+TEMPO_THRESHOLD) || (sum < previnterval-TEMPO_THRESHOLD)) && sum > 0)
						{
							playback->setPlaybackRate(sum);
						}
						count =0;
						bpm = (1000/(float)sum) * 60;
						previnterval = sum;
						sum =0;
					}
				}
			}
			else
			{
				if(y > 1)
				{
					decY = false;
					now = SDL_GetTicks();
					interval[count] = now - prev;
					prev = now;
					//printf("Direction changed to 1. Y is decreasing(upscreen), time interval %i\n", interval[count]);
					count++;

					if(count > TIME_SAMPLES-1)
					{
						for(i=0;i<count;i++)
						{
							if(i == 0)
							{
								min = 0;
								max = 0;
							} 
							else
							{
								if(interval[i] > interval[max])
								{
									max = count;
								}
								if(interval[i] < interval[min])
								{
									min = count;
								}
							}
						}

						for(i=0;i<count;i++)
						{
							if(i != max && i != min)
							{
								sum += interval[i];
							}
						}

						sum /= TIME_SAMPLES-2;

						if((sum > previnterval+TEMPO_THRESHOLD) || (sum < previnterval-TEMPO_THRESHOLD))
						{
							playback->setPlaybackRate(sum);
						}
						count =0;
						bpm = (1000/(float)sum) * 60;
						previnterval = sum;
						sum =0;
					}
				}
			} 
			prevPos = rhPos;
			break;
		}

		/*if(!(pixels.empty()))
		{
			IplImage image = pixels;
			gui->drawSurfaceFromMatrix(image);
			tempo = boost::lexical_cast<std::string>(bpm);
			gui->drawTempoString(tempo);
			gui->displayFrame();
			printf("Current pos: %f\r", rhPos);
		}
		else
		{
			printf("\n\nEmpty pixel frame\n\n");
		}*/

		gui->displayFrame();
	}

	delete(kinect);
	delete(gui);
	delete(playback);

	return 0;
}

string chooseFile()
{
	string path;

	fs::path xmlDir(XML_PATH);
	fs::directory_iterator end_iter;

	typedef multimap<int, fs::path> result_set_t;
	result_set_t result_set;
	result_set_t::iterator result_it;

	int count = 0;

	if( fs::exists(XML_PATH) && fs::is_directory(XML_PATH) )
	{
		for( fs::directory_iterator it(XML_PATH); it != end_iter; ++it)
		{
			if( fs::is_regular_file(it->status()) )
			{
				result_set.insert(result_set_t::value_type(++count, *it));
			}
		}
	}
	else
	{
		return "error, path specified is not a directory or does not exist";
	}

	count = 0;

	for( result_it = result_set.begin(); result_it != result_set.end(); result_it++)
	{
		printf("\n%i %s\n", ++count, result_it->second.string().c_str());
	}
	
	scanf_s(" %i", &count);

	path = result_set.find(count)->second.string();
	if(path != "")
	{
		return path;
	}
	else
	{
		return "Number not listed";
	}
}

bool checkPressTime(Uint32 &startTime)
{
	if(startTime == 0)
		startTime = SDL_GetTicks();
	else if((SDL_GetTicks() - startTime) > HOVER_TIME)
		return true;

	return false;
}
