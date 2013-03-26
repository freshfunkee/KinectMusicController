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

#include "KinectController.h"
#include "Gui.h"
#include "SongPlayback.h"
#include "HandMonitor.h"

#define XML_PATH "..\\songs"

using namespace std;
namespace fs = boost::filesystem;

string chooseFile();

int main()
{
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	printf("--Kinect Music Controller\n--Version: 0.5\n--Authour: Eoin Joyce\n--Features:"
				"\n\t*Echo, Flange, Tremolo Effect\n\t*Background change\n\t*Clap Detector\n");
	printf("\n\nWelcome! Select a song to play:");

	string path = chooseFile();

	Song *trackSelection = new Song();
	trackSelection->readSongXml(path.c_str());

	cout << "\nTrack:\t" << trackSelection->getTitle() << "\nArtist:\t" << trackSelection->getArtist() << endl;

	SongPlayback *playback = new SongPlayback(trackSelection);

	Gui *gui = new Gui();
	HandMonitor *monitor = new HandMonitor(playback, gui);
	KinectController *kinect = new KinectController(monitor);

	gui->initialize();
	kinect->initialize();
	playback->initialize();

	delete(trackSelection);

	printf("\n\n");

	kinect->run();

	delete playback;
	delete gui;
	delete monitor;
	delete kinect;

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
