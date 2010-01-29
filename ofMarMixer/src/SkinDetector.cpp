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
	bSkinBinary = false;
}

//--------------------------------------------------------------
SkinDetector::~SkinDetector()
{
	destroy();
}

void SkinDetector::setup()
{
#ifdef USE_CVD
	imBlur.		resize(videoSize);
	imSkin.		resize(videoSize);
	imMedian.	resize(videoSize);
	imDilated.	resize(videoSize);
	imEroded.	resize(videoSize);
#endif
#ifdef USE_OPENCV
	iplBlur		= cvCreateImageHeader(cvSize(videoSize.x,videoSize.y), IPL_DEPTH_8U, 3);	
	iplSkin		= cvCreateImageHeader(cvSize(videoSize.x,videoSize.y), IPL_DEPTH_8U, 1);
	iplMedian	= cvCreateImageHeader(cvSize(videoSize.x,videoSize.y), IPL_DEPTH_8U, 1);
	iplDilated	= cvCreateImageHeader(cvSize(videoSize.x,videoSize.y), IPL_DEPTH_8U, 1);
	iplEroded	= cvCreateImageHeader(cvSize(videoSize.x,videoSize.y), IPL_DEPTH_8U, 1);
#endif
}

//--------------------------------------------------------------
void SkinDetector::destroy()
{}

//--------------------------------------------------------------
void SkinDetector::update()
{
#ifdef USE_CVD
	if (!getWidth() || !getHeight())
		return;
	
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
	
#ifdef USE_OPENCV
	cvSetData(iplBlur,		imBlur.data(),		3*VIDEO_WIDTH);
	cvSetData(iplSkin,		imSkin.data(),		VIDEO_WIDTH);
	cvSetData(iplMedian,	imMedian.data(),	VIDEO_WIDTH);
	cvSetData(iplDilated,	imDilated.data(),	VIDEO_WIDTH);
	cvSetData(iplEroded,	imEroded.data(),	VIDEO_WIDTH);
#endif	
}
