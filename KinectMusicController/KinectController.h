#ifndef KINECTCONTROLLER_H
#define KINECTCONTROLLER_H

#include <Windows.h>
#include <sstream>
#include <NuiApi.h>

class HandMonitor;

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
	KinectController(HandMonitor*);
	~KinectController();

	void initialize();
	void run();
	void getJointValues(long&,long&,long&,Vector4);

private:
	INuiSensor *kinect_;
	HANDLE  skeletonEvent_;
	long **skelMatrix_;
	HandMonitor *monitor_;

	DWORD width, height;
	void createInstance();
	void errchk(HRESULT);
};

#endif