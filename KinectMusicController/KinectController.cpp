#include "KinectController.h"
#include "CVImage.h"

KinectController::KinectController(CVImage *image)
{
	cvImage_ = image;
}

KinectController::~KinectController()
{
	if(kinect_ != NULL)
	{
		kinect_->NuiShutdown();
		kinect_->Release();
	}
}

void KinectController::initialize()
{
	createInstance();

	ERROR_CHECK( kinect_->NuiInitialize( NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON ) );

	ERROR_CHECK( kinect_->NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR, CAMERA_RESOLUTION, 0, 2, 0, &imageStreamHandle_ ) );

	ERROR_CHECK( kinect_->NuiSkeletonTrackingEnable( 0, NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA ) );

	streamEvent_ = ::CreateEvent( 0, TRUE, FALSE, 0 );
	ERROR_CHECK( kinect_->NuiSetFrameEndEvent( streamEvent_, 0 ) );

	::NuiImageResolutionToSize(CAMERA_RESOLUTION, width, height );

	cvImage_->setScreenDimensions( width, height );
}

float KinectController::getHandRightPos()
{
	::WaitForSingleObject( streamEvent_, INFINITE );
	::ResetEvent( streamEvent_ );

	skeletonVector(handRight_, true);

	::NuiTransformSkeletonToDepthImage( handRight_, &depthX_, &depthY_, CAMERA_RESOLUTION );

	return depthY_;
}

float KinectController::getHandLeftPos()
{
	::WaitForSingleObject( streamEvent_, INFINITE );
	::ResetEvent( streamEvent_ );

	skeletonVector(handLeft_, false);

	::NuiTransformSkeletonToDepthImage( handLeft_, &depthX_, &depthY_, CAMERA_RESOLUTION );

	return depthY_;
}

void KinectController::getPixelMap()
{
	mapPixels();
}

void KinectController::createInstance()
{
	int count = 0;
    ERROR_CHECK( ::NuiGetSensorCount( &count ) );
    if ( count == 0 ) {
      throw std::runtime_error( "Kinect not connected" );
    }

    ERROR_CHECK( ::NuiCreateSensorByIndex( 0, &kinect_ ) );

    HRESULT status = kinect_->NuiStatus();
    if ( status != S_OK ) {
      throw std::runtime_error( "Kinect not available" );
    }
}

void KinectController::mapPixels()
{
	NUI_IMAGE_FRAME imageFrame = { 0 };
	ERROR_CHECK( kinect_->NuiImageStreamGetNextFrame(imageStreamHandle_, INFINITE, &imageFrame ) );
	NUI_LOCKED_RECT colorData;
	imageFrame.pFrameTexture->LockRect( 0, &colorData, 0, 0 );

	long depthPoints[4] = {};
	long *p = depthPoints;
	long *q = p+2;
	getJointLocation( handRight_, p );
	getJointLocation( handLeft_, q );

	cvImage_->mapKinectImage( colorData.pBits, depthPoints );
	/*image = cv::Mat( height, width, CV_8UC4, colorData.pBits );
	drawJoint( image, handRight_ );
	drawJoint( image, handLeft_ );
	drawPlayButton( image );*/

	ERROR_CHECK( kinect_->NuiImageStreamReleaseFrame( imageStreamHandle_, &imageFrame ) );
}

void KinectController::skeletonVector( Vector4 &hPos, bool hand )
{
	NUI_SKELETON_FRAME skeletonFrame = { 0 };
	kinect_->NuiSkeletonGetNextFrame( 0, &skeletonFrame );

	NUI_SKELETON_DATA *skeletonData = 0;
	for( int i =0; i < NUI_SKELETON_COUNT; ++i)
	{
		NUI_SKELETON_DATA &data = skeletonFrame.SkeletonData[i];
		if( data.eTrackingState == NUI_SKELETON_TRACKED )
		{
			skeletonData = &data;
			break;
		}
	}

	if( skeletonData == 0 )
		return;

	if(hand)
	{
		if( skeletonData->eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HAND_RIGHT]
			== NUI_SKELETON_POSITION_NOT_TRACKED )
				return;

		hPos = skeletonData->SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
	}
	else
	{
		if( skeletonData->eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HAND_LEFT]
			== NUI_SKELETON_POSITION_NOT_TRACKED )
				return;

		hPos = skeletonData->SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];
	}
}

//void KinectController::drawJoint( cv::Mat& image, Vector4 position )
//{
//    ::NuiTransformSkeletonToDepthImage( position, &depthX_, &depthY_, CAMERA_RESOLUTION );
//
//    LONG colorX = 0;
//    LONG colorY = 0;
//
//	kinect_->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
//      CAMERA_RESOLUTION, CAMERA_RESOLUTION,
//      0, (LONG)depthX_ , (LONG)depthY_, 0, &colorX, &colorY );
//
//    cv::circle( image, cv::Point( colorX, colorY ), 10, cv::Scalar( 0, 255, 0 ), 5 );
//}

//void KinectController::drawPlayButton( cv::Mat& image )
//{
//	cv::rectangle( image, cv::Rect( 50, 25, 50, 50 ), cv::Scalar( 0, 255, 0 ), 3 );
//}

void KinectController::getJointLocation( Vector4 &position, long *colorPoint )
{
	::NuiTransformSkeletonToDepthImage( position, &depthX_, &depthY_, CAMERA_RESOLUTION );

	kinect_->NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
      CAMERA_RESOLUTION, CAMERA_RESOLUTION,
      0, (LONG)depthX_ , (LONG)depthY_, 0, colorPoint, colorPoint++ );
}