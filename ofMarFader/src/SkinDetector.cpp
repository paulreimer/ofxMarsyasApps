#include "SkinDetector.h"
#include "skinRCA.h"

#include <cvd/draw.h>
#include <cvd/image_convert.h>
#include <cvd/morphology.h>
#include <cvd/convolution.h>
#include <cvd/interpolate.h>

using namespace CVD;

//--------------------------------------------------------------
SkinDetector::SkinDetector()
{
	disableAllEvents();
	enableDrawEvents();
	
	bSkinBinary = false;
}

//--------------------------------------------------------------
SkinDetector::~SkinDetector()
{
	destroy();
}

void SkinDetector::setup()
{}

//--------------------------------------------------------------
void SkinDetector::destroy()
{}

//--------------------------------------------------------------
void SkinDetector::update()
{
#ifdef USE_CVD
	if (!width || !height)
		return;
	else if (!videoSize.x && !videoSize.y)
		videoSize = ImageRef(width,height);
	
	if (imSkin.size() != videoSize)
	{
		imBlur.resize(videoSize);
		imSkin.resize(videoSize);
		imMedian.resize(videoSize);
		imDilated.resize(videoSize);
		imEroded.resize(videoSize);
	}
	
	BasicImage<Rgb<byte> > grabRGB((Rgb<byte>*)getPixels(), videoSize);
//	convolveGaussian(grabRGB, imBlur, 2.0);
//	median_filter_3x3(grabRGB, imBlur);
	
	if (bSkinBinary)
		convert_image<SkinRCA_binary<Rgb<byte>, byte>, Rgb<byte>, byte>(grabRGB, imSkin);
	else
		convert_image<SkinRCA<Rgb<byte>, byte>, Rgb<byte>, byte>(grabRGB, imSkin);
	
//	convolveGaussian(imSkin, 3.0);
	vector<ImageRef> element;
/*
	element = getDisc(5);
	morphology(imSkin, element, Morphology::Median<byte>(), imMedian);
*/
/*
	element = getDisc(2);
	morphology(imSkin, element, Morphology::BinaryDilate<>(), imDilated);

	element = getDisc(7);
	morphology(imDilated, element, Morphology::BinaryErode<>(), imEroded);
*/
#endif
}

//--------------------------------------------------------------
void SkinDetector::draw()
{}
