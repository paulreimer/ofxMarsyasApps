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
	run();
}

//--------------------------------------------------------------
void
SonificationEngine::update()
{
	if (responses==NULL || responses->empty())
		return;

	map<int, GeoData::response_t>::iterator resp_iter;
	vector<ofPoint>::iterator pt_iter;
	
	for (resp_iter = responses->begin(); resp_iter != responses->end(); resp_iter++)
	{
		cout << "Response to query " << resp_iter->first << endl;
		vector<ofPoint>& points = resp_iter->second.points;

		for (pt_iter = points.begin(); pt_iter != points.end(); pt_iter++)
			cout << "Data point: (" << pt_iter->x << "," << pt_iter->x << ")" << endl;
		
		responses->erase(resp_iter);
	}
}

//--------------------------------------------------------------
void
SonificationEngine::draw()
{}
