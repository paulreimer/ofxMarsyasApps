#include "FiducialTracker.h"

#ifdef USE_CVD
#include <cvd/image_convert.h>
#include <cvd/vision.h>
#include <cvd/gl_helpers.h>
using namespace CVD;
#endif

//--------------------------------------------------------------
FiducialTracker::FiducialTracker()
{
	frameCount	= 0;
	frameIncr	= 1;
	
	bTrackFiducials = true;
	bDrawFiducials = true;
	
	threshold = 90;

	blocksize	= 9;
	offset		= 0;
	invert		= false;
	gauss		= false;
}

//--------------------------------------------------------------
FiducialTracker::~FiducialTracker()
{
	destroy();
}

void FiducialTracker::setup()
{
#ifdef USE_CVD
	imBW.		resize(videoSize);
	imThreshold.resize(videoSize);
#endif
#ifdef USE_OPENCV
	iplBW		= cvCreateImageHeader(cvSize(videoSize.x,videoSize.y), IPL_DEPTH_8U, 1);
	iplThreshold= cvCreateImageHeader(cvSize(videoSize.x,videoSize.y), IPL_DEPTH_8U, 1);
#endif

	//detect finger is off by default
	tracker.detectFinger		= false;
	tracker.maxFingerSize		= 25;
	tracker.minFingerSize		= 5;
	tracker.fingerSensitivity	= 0.05f; //from 0 to 2.0f
	
//	fiducial_physics.tracker = &tracker;
//	fiducial_physics.ignoreHitFiducialIds.push_back(NULL_FIDUCIAL_ID);
}

//--------------------------------------------------------------
void FiducialTracker::destroy()
{	
//	fiducial_physics.tracker = NULL;
}

//--------------------------------------------------------------
void FiducialTracker::update()
{
	if (!getWidth() || !getHeight())
		return;

#ifdef USE_CVD
	BasicImage<Rgb<byte> > grabRGB((Rgb<byte>*)getPixels(), videoSize);
	convert_image(grabRGB, imBW);
#endif

#ifdef USE_OPENCV
	cvSetData(iplBW,			imBW.data(),		VIDEO_WIDTH);
	cvSetData(iplThreshold,		imThreshold.data(),	VIDEO_WIDTH);
#endif
	
#ifdef PREFER_OPENCV
	blocksize = (blocksize < 2)? 3 : !(blocksize % 2)? ++blocksize : blocksize;
	
	int threshold_type	= invert?	CV_THRESH_BINARY			: CV_THRESH_BINARY_INV;
    int adaptive_method	= gauss?	CV_ADAPTIVE_THRESH_MEAN_C	: CV_ADAPTIVE_THRESH_GAUSSIAN_C;

    cvAdaptiveThreshold(iplBW, iplThreshold,
						255, adaptive_method,
						threshold_type,
						blocksize, offset);
#else
	imThreshold.copy_from(imBW);
	CVD::threshold(imThreshold, (byte)(0xff&threshold), (byte)0xff);
#endif
	
	if (bTrackFiducials)
		tracker.findFiducials(imThreshold.data(), videoSize.x, videoSize.y);
	/*
	if (width != fiducial_physics.width || height != fiducial_physics.height)
	{
		fiducial_physics.width	= width;
		fiducial_physics.height	= height;
		fiducial_physics.setup();
	}

	fiducial_physics.update();
*/
}

//--------------------------------------------------------------
void FiducialTracker::draw(float x, float y)
{
	draw(x, y, getWidth(), getHeight());
}

//--------------------------------------------------------------
void FiducialTracker::draw(float x, float y, float w, float h)
{
	glPushMatrix();
	glTranslatef(x, y, 0.);
#ifdef USE_CVD
	glScalef(w/getWidth(), h/getHeight(), 1.);
#endif

	if (bTrackFiducials && bDrawFiducials)
	{
		ofSetColor(255,255,255);
		for (list<ofxFiducial>::iterator fiducial = tracker.fiducialsList.begin();
			 fiducial != tracker.fiducialsList.end();
			 fiducial++)
		{
			ofSetColor(0x00ff00);
			fiducial->draw(0,0);
			ofSetColor(0x0000ff);
			fiducial->drawCorners(0,0);
		}
		
		//draw the fingers
		for (list<ofxFinger>::iterator finger = tracker.fingersList.begin();
			 finger != tracker.fingersList.end();
			 finger++)
		{
			ofSetColor(0xff0000);
			finger->draw(0,0);
		}
	}
	
	glPopMatrix();
}
