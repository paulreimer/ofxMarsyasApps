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
	
	bTrackFiducials = false;
	bDrawFiducials = false;
	
	_threshold = 90;
	
	disableAllEvents();
	enableDrawEvents();
}

//--------------------------------------------------------------
FiducialTracker::~FiducialTracker()
{
	destroy();
}

void FiducialTracker::setup()
{
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
	if (!width || !height)
		return;
	else if (!videoSize.x && !videoSize.y)
		videoSize = ImageRef(width,height);

#ifdef USE_CVD
	if (imBW.size() != videoSize)
	{
		imBW.resize(videoSize);
		imThreshold.resize(videoSize);
	}

	BasicImage<Rgb<byte> > grabRGB((Rgb<byte>*)getPixels(), videoSize);
	convert_image(grabRGB, imBW);
	imThreshold.copy_from(imBW);

	threshold(imThreshold, (byte)(0xff&_threshold), (byte)0xff);

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
#endif
}

//--------------------------------------------------------------
void FiducialTracker::draw()
{
	glPushMatrix();
	glTranslatef(x, y, 0.);
	glScalef(width/videoSize.x, height/videoSize.y, 1.);

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
