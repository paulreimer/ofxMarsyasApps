#include "FiducialTracker.h"

//--------------------------------------------------------------
FiducialTracker::FiducialTracker()
{
	frameCount	= 0;
	frameIncr	= 1;
	
	bTrackFiducials = true;
	bDrawFiducials = true;
	
	threshold = 90;

	blocksize	= 15;
	offset		= 2;
	invert		= true;
	gauss		= false;
}

//--------------------------------------------------------------
FiducialTracker::~FiducialTracker()
{
	destroy();
}

void FiducialTracker::setup()
{
#ifdef USE_OPENCV
	imThreshold.allocate(VIDEO_SIZE);
	imThreshold.set(0);
#endif

	//detect finger is off by default
	tracker.detectFinger		= false;
	tracker.maxFingerSize		= 25;
	tracker.minFingerSize		= 5;
	tracker.fingerSensitivity	= 0.05f; //from 0 to 2.0f
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

#ifdef USE_OPENCV
	int flipMode;

	if		( MIRROR_VERTICAL && !MIRROR_HORIZONTAL) flipMode = 0;
	else if	(!MIRROR_VERTICAL &&  MIRROR_HORIZONTAL) flipMode = 1;
	else if	( MIRROR_VERTICAL &&  MIRROR_HORIZONTAL) flipMode = -1;

	if (MIRROR_VERTICAL || MIRROR_HORIZONTAL)
		cvFlip(imBW->getCvImage(), NULL, flipMode);

	blocksize = (blocksize < 2)? 3 : !(blocksize % 2)? ++blocksize : blocksize;
	
	int threshold_type	= invert?	CV_THRESH_BINARY			: CV_THRESH_BINARY_INV;
    int adaptive_method	= gauss?	CV_ADAPTIVE_THRESH_MEAN_C	: CV_ADAPTIVE_THRESH_GAUSSIAN_C;

    cvAdaptiveThreshold(imBW->getCvImage(), imThreshold.getCvImage(),
						255, adaptive_method,
						threshold_type,
						blocksize, offset);
	
	imThreshold.flagImageChanged();
#endif
	
	if (bTrackFiducials)
		tracker.findFiducials((unsigned char*)imThreshold.getCvImage()->imageData, videoSize.x, videoSize.y);
	
	if (!tracker.fiducialsList.empty())
		ofNotifyEvent(newFrame, tracker.fiducialsList);
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
	glScalef(w/getWidth(), h/getHeight(), 1.);

	if (bTrackFiducials && bDrawFiducials)
	{
		ofSetColor(255,255,255);
		for (list<ofxFiducial>::iterator fiducial = tracker.fiducialsList.begin();
			 fiducial != tracker.fiducialsList.end();
			 fiducial++)
		{
			if (fiducial->getX() == FIDUCIAL_INVALID_COORDS
				&& fiducial->getY() == FIDUCIAL_INVALID_COORDS)
				continue;

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
