#include "CVImage.h"
#include "KinectController.h"
#include "Gui.h"
#include "SongPlayback.h"

#define JOINT_RADIUS 5
#define JOINT_COLOR_R 255
#define JOINT_COLOR_G 255
#define JOINT_COLOR_B 255

#define PLAY_COLOR_R 0
#define PLAY_COLOR_G 255
#define PLAY_COLOR_B 0

#define PAUSE_COLOR_R 255
#define PAUSE_COLOR_G 0
#define PAUSE_COLOR_B 0

CVImage::CVImage(SongPlayback *playback)
{
	playback_ = playback;
	jointColor_ = cv::Scalar( JOINT_COLOR_R, JOINT_COLOR_G, JOINT_COLOR_B );
	playButtonColor_ = cv::Scalar( PLAY_COLOR_R, PLAY_COLOR_G, PLAY_COLOR_B );
	pauseButtonColor_ = cv::Scalar( PAUSE_COLOR_R, PAUSE_COLOR_G, PAUSE_COLOR_B );
}

CVImage::~CVImage()
{
}

void CVImage::mapKinectImage( byte *colorBits, long *depthPoint )
{
	/*Creates matrix specified by width and height, 
		matrix type (8bit, 4channel) and color bytes
	*/
	pixels_ = cv::Mat( screenHeight_, screenWidth_, CV_8UC4, colorBits );

	/*Draws left and right hand joints to the matrix
	*/
	cv::circle( pixels_, cv::Point( *depthPoint, *depthPoint++), JOINT_RADIUS, jointColor_, 2 );
	depthPoint++;
	cv::circle( pixels_, cv::Point( *depthPoint, *depthPoint++), JOINT_RADIUS, jointColor_, 2 );
	//drawPlayButton();
}

void CVImage::setScreenDimensions( DWORD &width, DWORD &height )
{
	screenWidth_ = width;
	screenHeight_ = height;
}

void CVImage::getPixelMap( IplImage &image )
{
	image = pixels_;
}

void CVImage::drawPlayButton()
{
	if(playback_->getPlaybackState() == eStreamPlaying)
		cv::rectangle( pixels_, cv::Rect( 50, 25, 50, 50 ), playButtonColor_, 3 );
	else
		cv::rectangle( pixels_, cv::Rect( 50, 25, 50, 50 ), pauseButtonColor_, 3 );
}