#ifndef CVIMAGE_H
#define CVIMAGE_H

class KinectController;
class Gui;
class SongPlayback;

#include <opencv2\opencv.hpp>
#include <Windows.h>
#include <NuiApi.h>

class CVImage
{
public:
	CVImage(SongPlayback*);
	~CVImage();

	void getPixelMap( IplImage& );
	void setScreenDimensions( DWORD&, DWORD& );
	void mapKinectImage( byte*, long* );

private:
	void drawPlayButton();
	void drawJoint( cv::Mat&, Vector4 );

	cv::Mat pixels_;
	int screenWidth_, screenHeight_;
	cv::Scalar jointColor_;
	cv::Scalar playButtonColor_;
	cv::Scalar pauseButtonColor_;

	SongPlayback *playback_;
};
#endif