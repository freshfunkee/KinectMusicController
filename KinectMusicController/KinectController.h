#ifndef KINECTCONTROLLER_H
#define KINECTCONTROLLER_H

class CVImage;
#include <Windows.h>

#include <NuiApi.h>

#define ERROR_CHECK( ret )  \
  if ( ret != S_OK ) {      \
    std::stringstream ss;	  \
    ss << "failed " #ret " " << std::hex << ret << std::endl;			\
    throw std::runtime_error( ss.str().c_str() );			\
  }

const NUI_IMAGE_RESOLUTION CAMERA_RESOLUTION = NUI_IMAGE_RESOLUTION_640x480;

class KinectController
{
public:
	KinectController(CVImage*);
	~KinectController();

	void initialize();
	void getPixelMap();
	float getHandRightPos();
	float getHandLeftPos();

private:
	INuiSensor *kinect_;
	HANDLE imageStreamHandle_, septhStreamImageHandle_, streamEvent_;
	CVImage *cvImage_;

	float depthX_, depthY_;
	Vector4 handRight_, handLeft_;

	DWORD width, height;

	void createInstance();
	void mapPixels();
	void skeletonVector(Vector4&, bool);
	
	void getJointLocation( Vector4&, long* );

};

#endif