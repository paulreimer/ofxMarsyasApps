#include "SonificationEngine.h"

//--------------------------------------------------------------
SonificationEngine::SonificationEngine(string name)
: ofxMarsyasNetwork(name)
{
	priority = 2;

	disableAllEvents();
	ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
	ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);
}

//--------------------------------------------------------------
void
SonificationEngine::setup()
{
//	run();
}

//--------------------------------------------------------------
void
SonificationEngine::update()
{
	if (fiducials==NULL)
		return;
}

//--------------------------------------------------------------
void
SonificationEngine::draw()
{}
