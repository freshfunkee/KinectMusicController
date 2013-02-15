#include "KinectController.h"

#include "Gui.h"

#include <conio.h>

KinectController::KinectController(Gui *gui)
{
	gui_ = gui;

	skelMatrix_ = new long *[NUI_SKELETON_POSITION_COUNT];

	for(int i=0;i<NUI_SKELETON_POSITION_COUNT;i++)
    {
        skelMatrix_[i] = new long[3];
    }
}

KinectController::~KinectController()
{
	if(kinect_ != NULL)
	{
		kinect_->NuiShutdown();
		kinect_->Release();
	}

	for(int i=0;i<NUI_SKELETON_POSITION_COUNT;i++)
    {
        delete [] skelMatrix_[i];
    }

    delete [] skelMatrix_;
}

void KinectController::initialize()
{
	createInstance();

	ERROR_CHECK( kinect_->NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON ) );

	skeletonEvent_ = CreateEvent( 0, TRUE, FALSE, 0 );

	ERROR_CHECK( kinect_->NuiSkeletonTrackingEnable( skeletonEvent_, NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA ) );

	ERROR_CHECK( kinect_->NuiSetFrameEndEvent( skeletonEvent_, 0 ) );

	NuiImageResolutionToSize(CAMERA_RESOLUTION, width, height );

	//->SDL Image dimensions instead of opencv
}

void KinectController::run()
{
	while(!(_kbhit()))
	{
		NUI_SKELETON_FRAME skeletonFrame = { 0 };
		kinect_->NuiSkeletonGetNextFrame( 0, &skeletonFrame );

		NUI_SKELETON_DATA *skeletonData = 0;
		for ( int i = 0; i < NUI_SKELETON_COUNT; ++i ) 
		{
			NUI_SKELETON_DATA& skeletonData = skeletonFrame.SkeletonData[i];
			if ( skeletonData.eTrackingState == NUI_SKELETON_TRACKED ) 
			{
				for ( int j = 0; j < NUI_SKELETON_POSITION_COUNT; ++j ) 
				{
					if ( skeletonData.eSkeletonPositionTrackingState[j] != NUI_SKELETON_POSITION_NOT_TRACKED ) 
					{
						getJointValues(skelMatrix_[j][0], skelMatrix_[j][1], skelMatrix_[j][2], skeletonData.SkeletonPositions[j]);
					}
				}
			}
			else if ( skeletonData.eTrackingState == NUI_SKELETON_POSITION_ONLY ) 
			{
				//if skeleton is not tracked
			}
		}

		gui_->displayFrame(skelMatrix_);
		Sleep(17);
	}
}

void KinectController::getJointValues(long &x, long &y, long &z, Vector4 joint)
{
	USHORT short_z = (USHORT)z;

	NuiTransformSkeletonToDepthImage( joint, &x, &y, &short_z, CAMERA_RESOLUTION );

	z = (long)short_z;
}

void KinectController::createInstance()
{
	int count = 0;
    ERROR_CHECK( NuiGetSensorCount( &count ) );
    if ( count == 0 ) {
      throw std::runtime_error( "Kinect not connected" );
    }

    ERROR_CHECK( NuiCreateSensorByIndex( 0, &kinect_ ) );

    HRESULT status = kinect_->NuiStatus();
    if ( status != S_OK ) {
      throw std::runtime_error( "Kinect not available" );
    }
}