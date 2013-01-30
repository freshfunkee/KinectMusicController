#include "Song.h"

bool Song::readSongXml(const char* file)
{
	TiXmlDocument doc(file);
	if(!doc.LoadFile()) return false;

	TiXmlElement *elSong = doc.FirstChildElement("song");

	if(elSong)
	{
		TiXmlElement *elTitle = elSong->FirstChildElement("title");
		title_ = elTitle->GetText();

		elTitle = elSong->FirstChildElement("artist");
		artist_ = elTitle->GetText();

		elTitle = elSong->FirstChildElement("album");
		album_ = elTitle->GetText();

		elTitle = elSong->FirstChildElement("path");
		path_ = elTitle->GetText();

		elTitle = elSong->FirstChildElement("bpm");
		string bpm_str = elTitle->GetText();
		bpm_ = atof(bpm_str.c_str());

		return true;
	}
	else return false;
}