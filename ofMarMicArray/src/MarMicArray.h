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
	
	float angle;
protected:
	int nTicks;

	int window_size;
	double fs;
	double c; 
	int iterations;

	Marsyas::realvec inData0, inData1, inData2, inData3;
	Marsyas::realvec outData01, outData02, outData03, outData12, outData13, outData23;

	float pos0, pos1, pos2, pos3;
	float thres01, thres02, thres03, thres12, thres13, thres23;
	float t01,t02,t03,t12,t13,t23;
	float angle01,angle02,angle03,angle12,angle13,angle23;

	float max, maxArg;
};
