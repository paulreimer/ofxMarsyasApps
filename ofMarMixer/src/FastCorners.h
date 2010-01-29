#pragma once

#include "settings.h"

#ifdef USE_CVD
#include "ofxCvdImage.h"
#endif

#ifdef USE_OPENCV
#include "ofxCvMain.h"
#endif

#include "ofxMSAInteractiveObject.h"

class FastCorners
: public ofBaseUpdates
, public ofBaseHasPixels
, public ofBaseDraws
{
public:
	FastCorners();
	~FastCorners();
	
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
	CVD::ImageRef			videoSize;
#else
	ofPoint					videoSize;
#endif

	bool bFindCorners;

	int barrier;
	int nFoundCorners;
	bool bStrict;
	
	std::vector<CVD::ImageRef> all_corners;
	std::vector<CVD::ImageRef> corners;
	std::vector<int> scores;	
	
	unsigned char* pixels;
};
