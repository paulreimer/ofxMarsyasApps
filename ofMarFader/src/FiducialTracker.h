#pragma once

#define USE_CVD

#include "ofxMSAInteractiveObject.h"

#ifdef USE_CVD
#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
#endif

#define NULL_FIDUCIAL_ID 1
#include "ofxFiducialTracker.h"
//#include "PhysicsEngine.h"

class FiducialTracker : public ofxMSAInteractiveObject, public ofBaseHasPixels
{
public:
	FiducialTracker();
	~FiducialTracker();
	
	void setup();
	void destroy();
	
	void update();
	void draw();

	unsigned char * getPixels() { return pixels; };

#ifdef USE_CVD	
	CVD::Image<CVD::byte>	imBW;
	CVD::Image<CVD::byte>	imThreshold;
	CVD::ImageRef			videoSize;
#endif
	
	int _threshold;
	
	unsigned char frameCount, frameIncr;
	
	bool bTrackFiducials;
	bool bDrawFiducials;

	unsigned char* pixels;
	//private:
//	ofxFiducialBacked::physics::Engine fiducial_physics;	
	ofxFiducialTracker tracker;
};
