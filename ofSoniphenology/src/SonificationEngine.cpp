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
	addMarSystem(mng.create("Gain", "g1"));

	run();
}

//--------------------------------------------------------------
void
SonificationEngine::update()
{
	if (geoData==NULL || geoData->responses.empty())
		return;

	map<int, GeoData::response_t>::iterator resp_iter;
	vector<ofPoint>::iterator pt_iter;

//	while (!geoData->lock())
//	{}

	geoData->lock();
	for (resp_iter = geoData->responses.begin(); resp_iter != geoData->responses.end(); resp_iter++)
	{
		cout << "Response to query " << resp_iter->first << endl;
		vector<ofPoint>& points = resp_iter->second.points;

		for (pt_iter = points.begin(); pt_iter != points.end(); pt_iter++)
			cout << "Data point: (" << pt_iter->x << "," << pt_iter->x << ")" << endl;
		
		geoData->responses.erase(resp_iter);
	}
	geoData->unlock();
}

//--------------------------------------------------------------
void
SonificationEngine::draw()
{}
