#pragma once

#define USE_CVD
#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>

#include "ofxMSAInteractiveObject.h"

class SkinDetector : public ofxMSAInteractiveObject, public ofBaseHasPixels
{
public:
	SkinDetector();
	~SkinDetector();
	
	void setup();
	void destroy();
	
	void update();
	void draw();
	
	unsigned char * getPixels() { return pixels; };
	
#ifdef USE_CVD
	CVD::Image<CVD::Rgb<CVD::byte> > imBlur;
	CVD::Image<CVD::byte>	imSkin;
	CVD::Image<CVD::byte>	imMedian;
	CVD::Image<CVD::byte>	imDilated;
	CVD::Image<CVD::byte>	imEroded;
	CVD::ImageRef			videoSize;
#endif

	bool bSkinBinary;

	unsigned char* pixels;
};
