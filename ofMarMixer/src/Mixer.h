#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"
#include "marsyas/Fanout.h"
#include "ofxFiducial.h"

class Mixer : public ofxMarsyasNetwork, public ofxMSAInteractiveObject
{
public:
	Mixer(string name = "mixer");
	
	void setup();
	void update();
	void draw();

	MarSystem* addChannel(int id, string inAudioFileName);
	void registerSourceTypeAsChannelType(string SourceSystem, string PrototypeSystem);

	Marsyas::MarSystem* channels;
	list<ofxFiducial>* fiducials;
};
