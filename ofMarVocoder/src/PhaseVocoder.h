#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"

class PhaseVocoder : public ofxMarsyasNetwork, public ofxMSAInteractiveObject
{
public:
	PhaseVocoder(string name = "pvseries");
	
	void setup();	
	void update();
	void draw();

	void mouseMoved(int x, int y);
	
	string inAudioFileName;
	string vocoderName;

	int fftSize;
	int windowSize;
	int bufferSize;
	int D;
	int I;
	int sinusoids;
	int V;
	bool oscbank_;
	
	Marsyas::mrs_real pitch;
	bool microphone_;
	Marsyas::mrs_string convertmode_;
};
