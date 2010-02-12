#pragma once

#include "ofxMarsyasNetwork.h"
#include "ofxMSAInteractiveObject.h"
#include "marsyas/Fanout.h"
#include "settings.h"
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
	void draw(float x, float y);
	void draw(float x, float y, float w, float h);

	GeoData* geoData;

	int tempo;
	float lerpFactor;
	map<int, string> instruments;
private:
	int nTicks;
	int position;
	float smoothedPosition;
};
