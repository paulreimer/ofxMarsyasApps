#pragma once

#include "settings.h"

#ifdef USE_CVD
#include "ofxCvdImage.h"
#endif

#ifdef USE_OPENCV
#include "ofxCvMain.h"
#endif

#include "ofxMSAInteractiveObject.h"


class SkinDetector
: public ofBaseUpdates
, public ofBaseHasPixels
{
public:
	SkinDetector();
	~SkinDetector();
	
	void setup();
	void destroy();
	
	void update();
	
	float getWidth() { return videoSize.x; }
	float getHeight() { return videoSize.y; }
	
	unsigned char * getPixels() { return pixels; };

#ifdef USE_OPENCV
	IplImage*				iplBlur;
	IplImage*				iplSkin;
	IplImage*				iplMedian;
	IplImage*				iplDilated;
	IplImage*				iplEroded;
#endif

#ifdef USE_CVD
	ofxCvdColorImage		imBlur;
	ofxCvdGrayscaleImage	imSkin;
	ofxCvdGrayscaleImage	imMedian;
	ofxCvdGrayscaleImage	imDilated;
	ofxCvdGrayscaleImage	imEroded;
	CVD::ImageRef			videoSize;
#else
	ofPoint					videoSize;
#endif

	bool bSkinBinary;

	unsigned char* pixels;
};
