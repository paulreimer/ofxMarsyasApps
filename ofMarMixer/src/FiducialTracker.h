#pragma once

#include "settings.h"

#include "ofxMSAInteractiveObject.h"

#include "ofxFiducialTracker.h"
#include "PhysicsEngine.h"

#ifdef USE_CVD
#include "ofxCvdImage.h"
#endif

#ifdef USE_OPENCV
#include "ofxCvMain.h"
#endif

class FiducialTracker
: public ofBaseUpdates
, public ofBaseHasPixels
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

	float getWidth() { return videoSize.x; }
	float getHeight() { return videoSize.y; }
	
	unsigned char * getPixels() { return pixels; };

#ifdef USE_CVD
	ofxCvdGrayscaleImage	imBW;
	ofxCvdGrayscaleImage	imThreshold;
	CVD::ImageRef			videoSize;
#else
	ofPoint					videoSize;
#endif

#ifdef USE_OPENCV
	IplImage* iplBW;
	IplImage* iplThreshold;
#endif

	int threshold;
	
	int blocksize;
	int offset;
	bool invert;
	bool gauss;
	
	unsigned char frameCount, frameIncr;
	
	bool bTrackFiducials;
	bool bDrawFiducials;

	unsigned char* pixels;
	//private:
//	ofxFiducialBacked::physics::Engine fiducial_physics;	
	ofxFiducialTracker tracker;
};
