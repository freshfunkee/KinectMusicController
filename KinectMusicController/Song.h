#ifndef SONG_H
#define SONG_H

#include <string>
#include <queue>

#include <tinyxml/tinyxml.h>
#include "Loop.h"

using namespace std;

class Song
{
public:
	Song()
		: bpm_(0), title_("-"), artist_("-"), album_("-"), path_(".")
	{
	}

	~Song() {}

	bool readSongXml(const char*);

	float getBpm() { return bpm_; }
	string getTitle() { return title_; }
	string getArtist() { return artist_; }
	string getAlbum() { return album_; }
	string getPath() { return path_; }
	std::queue<Loop> getLoop() { return loopQ_; }

private:
	float bpm_;
	string title_,artist_,album_,path_;
	std::queue<Loop> loopQ_;
	Loop loop_;
};

#endif