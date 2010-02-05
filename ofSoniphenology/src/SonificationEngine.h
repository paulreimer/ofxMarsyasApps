#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"
#include "marsyas/Fanout.h"

#include "GeoData.h"

class SonificationEngine
: public ofxMarsyasNetwork
, public ofxMSAInteractiveObject
{
public:
	SonificationEngine(string name = "SonificationEngine");
	
	void setup();
	void update();
	void draw();

	GeoData* geoData;

	int tempo;
	map<int, string>			instruments;
	map<int, map<int,int> >	offsets;
private:
	int nTicks;
	int position;
};
