#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"

class MarMicArray : public ofxMarsyasNetwork, public ofxMSAInteractiveObject
{
public:
	MarMicArray();
	~MarMicArray();
	
	void setup();
	void destroy();
	
	void update();
	void draw();
	
	double angle;
protected:
	int nTicks;

	int window_size;
	double fs;
	double c; 
	int iterations;
	
	// Position of microphones (relative linear array)
	// I need to correspond the position of the mics to the channels so I can use these
	
	double pos0;
	double pos1;
	double pos2;
	double pos3;
	
	// Time thresholds based on distance between each microphone
//	typedef double tdoa_thresh_t;
	typedef int tdoa_thresh_t;
	tdoa_thresh_t tdoa_thres01;
	tdoa_thresh_t tdoa_thres02;
	tdoa_thresh_t tdoa_thres03;
	tdoa_thresh_t tdoa_thres12;
	tdoa_thresh_t tdoa_thres13;
	tdoa_thresh_t tdoa_thres23;
	
	int thres01[2];
	int thres02[2];
	int thres03[2];
	int thres12[2];
	int thres13[2];
	int thres23[2];

	Marsyas::realvec outData01;
	Marsyas::realvec outData02;
	Marsyas::realvec outData03;
	Marsyas::realvec outData12;
	Marsyas::realvec outData13;
	Marsyas::realvec outData23;
};
