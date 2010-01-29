#pragma once

#define USE_CVD
#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>

#define FAST_CORNER_FUNC	fast_corner_detect_9
#define FAST_CORNER_SCORER	fast_corner_score_9

#include "ofxMSAInteractiveObject.h"

class FastCorners : public ofxMSAInteractiveObject, public ofBaseHasPixels
{
public:
	FastCorners();
	~FastCorners();
	
	void setup();
	void destroy();
	
	void update();
	void draw();
	
	unsigned char * getPixels() { return pixels; };
	
#ifdef USE_CVD	
	CVD::Image<CVD::byte>	imBW;
	CVD::ImageRef			videoSize;
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
