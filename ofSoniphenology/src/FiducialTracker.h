#pragma once

#include "settings.h"

#include "ofxMSAInteractiveObject.h"

#include "ofxFiducialTracker.h"

#ifdef USE_OPENCV
#include "ofxCvMain.h"
#endif

class FiducialTracker
: public ofBaseUpdates
, public ofBaseDraws
{
public:
	FiducialTracker();
	~FiducialTracker();
	
	void setup();
	void destroy();
	
	void update();
	void draw(float x, float y);
	void draw(float x, float y, float w, float h);

#ifdef USE_CV
#ifdef USE_OPENCV
	ofxCvGrayscaleImage	*imBW;
	ofxCvGrayscaleImage	imThreshold;
#endif
	ofPoint				videoSize;

	float getWidth() { return videoSize.x; }
	float getHeight() { return videoSize.y; }	
#else
	float getWidth() { return 0; }
	float getHeight() { return 0; }	
#endif

	int threshold;
	
	int blocksize;
	int offset;
	bool invert;
	bool gauss;
	
	unsigned char frameCount, frameIncr;
	
	bool bTrackFiducials;
	bool bDrawFiducials;

	//private:
//	ofxFiducialBacked::physics::Engine fiducial_physics;	
	ofxFiducialTracker tracker;
	
	ofEvent<list<ofxFiducial> > newFrame;
};
