#include "Song.h"

bool Song::readSongXml(const char* file)
{
	TiXmlDocument doc(file);
	if(!doc.LoadFile()) return false;

	TiXmlElement *elSong = doc.FirstChildElement("song");
	TiXmlNode *elLoop;

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

		for(elLoop = elSong->FirstChild("loop"); elLoop; elLoop = elLoop->NextSibling())
		{
			elTitle = elLoop->FirstChildElement("start");
			elTitle->QueryUnsignedAttribute("value", &loop_.start);

			elTitle = elLoop->FirstChildElement("end");
			elTitle->QueryUnsignedAttribute("value", &loop_.end);

			elTitle = elLoop->FirstChildElement("effect");
			elTitle->QueryUnsignedAttribute("value", &loop_.effect);

			elTitle = elLoop->FirstChildElement("threshold");
			elTitle->QueryFloatAttribute("value", &loop_.threshold);

			loopQ_.push(loop_);
		}
		
		return true;
	}
	else return false;
}