#include "SonificationEngine.h"
#include "msaColor.h"

#define INSTRUMENT_NOTE(i,n) string("instrument"+ofToString(i)+"_note"+ofToString(n))
/*
class slash_skipper
: public std::ctype<char> {
    mask ws_chars[1];
public:
	slash_skipper(size_t refs = 0)  
	: std::ctype<char>(ws_chars, false, refs) 
	{
		ws_chars['/'] = (mask)space;
	}
};
*/

//--------------------------------------------------------------
SonificationEngine::SonificationEngine(string name)
: ofxMarsyasNetwork(name)
{
	nTicks		= 0;
	position	= 0;
	tempo		= TEMPO_TICKS;

	priority	= 4;

	disableAllEvents();
	ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
//	ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);
}

//--------------------------------------------------------------
void
SonificationEngine::setup()
{
	Marsyas::MarSystem *bigfanout, *fanout, *source;

	bigfanout = mng.create("Fanout","bigfanout");
	addMarSystem(bigfanout);

	string filename;
	// Add the MarSystems
	map<int, string>::iterator instr_iter;
	for (instr_iter = instruments.begin(); instr_iter != instruments.end(); instr_iter++)
	{
		int instr = instr_iter->first;

		fanout = mng.create("Fanout", "fanout" + ofToString(instr));
		bigfanout->addMarSystem(fanout);

		for (int note=0; note<TIMESTAMP_RANGE_YEARS; note++)
		{
			source = mng.create("SoundFileSource", "src" + ofToString(note));
			fanout->addMarSystem(source);

			filename = ofToDataPath(instruments[instr]) + "/" + ofToString(note+1) + ".wav";

			linkctrl("mrs_bool/mute_"		+ INSTRUMENT_NOTE(instr,note),
					 source->getAbsPath()	+ "mrs_bool/mute");
			linkctrl("mrs_natural/pos_"		+ INSTRUMENT_NOTE(instr,note),
					 source->getAbsPath()	+ "mrs_natural/pos");

			source->updctrl("mrs_real/repetitions",	1.);
			source->updctrl("mrs_string/filename",	filename);
			source->updctrl("mrs_bool/mute",		true);
		}
	}

	addMarSystem(mng.create("MixToMono", "mono"));
	addMarSystem(mng.create("AudioSink", "dest"));
	updctrl("AudioSink/dest/mrs_bool/initAudio", true);
	
//	updctrl("mrs_natural/inSamples", 2048);

	run();
}

//--------------------------------------------------------------
void
SonificationEngine::update()
{
	nTicks++;
	// Change nTicks to change the tempo.  Bigger == slower
	if (nTicks % tempo == 0)
	{
		position++;
		cout << ".";
	}
	
	// Place to wrap around
	if ((position+1) % (TIMESTAMP_RANGE_DAYS + 5) == 0)
	{
		position = 0;
		cout << endl;
	}

	if (geoData==NULL || geoData->responses.empty())
		return;

	map<int, GeoData::response_t>::iterator resp_iter;

//	while (!geoData->lock())
//	{}
	
	int year_idx, date_idx;
	int instrument;

	map<int, GeoData::response_t>::iterator	instr_iter;
	map<int,int>::iterator					year_iter;

	if (nTicks % tempo != 0)
		return;

	geoData->lock();
	for (instr_iter = geoData->responses.begin();
		 instr_iter != geoData->responses.end();
		 instr_iter++)
	{
		instrument	= instr_iter->first;

		for (year_iter = instr_iter->second.offsets.begin();
			 year_iter != instr_iter->second.offsets.end();
			 year_iter++)
		{
			year_idx	= year_iter->first;
			date_idx	= year_iter->second;

			if (date_idx == position)
			{
				updctrl("mrs_bool/mute_"	+ INSTRUMENT_NOTE(instrument,year_idx),	false);
				updctrl("mrs_natural/pos_"	+ INSTRUMENT_NOTE(instrument,year_idx),	0);
				cout << instrument << "," << year_idx;
			}
		}
	}
	geoData->unlock();
}


//--------------------------------------------------------------
void SonificationEngine::draw()
{
	draw(0,0, ofGetWidth(), ofGetWidth());
}

//--------------------------------------------------------------
void SonificationEngine::draw(float x, float y)
{
	draw(x, y, ofGetWidth(), ofGetWidth());
}

//--------------------------------------------------------------
void SonificationEngine::draw(float x, float y, float w, float h)
{
	int year_idx, date_idx;
	int instrument;
	
	map<int, GeoData::response_t>::iterator	instr_iter;
	map<int,int>::iterator					year_iter;	
	
	int roygbiv = 0;
	msaColor color;

	glPushMatrix();
	glTranslatef(x, y+h, 0.);
	glScalef(1., -1., 1.);
	
	ofSetColor(0x777777);
	for (int i=0; i<TIMESTAMP_RANGE_YEARS; i++)
	{
		if (i%2)
			ofSetLineWidth(2.0);
		else
			ofSetLineWidth(1.0);

		ofLine(0, i*(h/TIMESTAMP_RANGE_YEARS),
			   w, i*(h/TIMESTAMP_RANGE_YEARS));
	}
	ofSetLineWidth(1.0);

	geoData->lock();
	for (instr_iter = geoData->responses.begin();
		 instr_iter != geoData->responses.end();
		 instr_iter++)
	{
		instrument	= instr_iter->first;
		roygbiv = (roygbiv+1) % 7;

		color.setHSV(roygbiv*360., 1., 1., 1.);
		color.setGL();

		for (year_iter = instr_iter->second.offsets.begin();
			 year_iter != instr_iter->second.offsets.end();
			 year_iter++)
		{
			year_idx	= year_iter->first;
			date_idx	= year_iter->second;
			
			ofCircle(ofMap(date_idx, 0, TIMESTAMP_RANGE_DAYS,	0, w),
					 ofMap(year_idx, 0, TIMESTAMP_RANGE_YEARS,	0, h),
					 (h/TIMESTAMP_RANGE_YEARS)-1);
		}
	}
	geoData->unlock();
	
	if (position < smoothedPosition)
		smoothedPosition = position;
	else
		smoothedPosition = ofLerp(smoothedPosition, position, 0.05);

	float sequencerPos = ofMap(smoothedPosition, 0, TIMESTAMP_RANGE_DAYS, 0, w);
	
	ofSetColor(0xdadada);
	ofSetLineWidth(3.0);
	ofLine(sequencerPos, 0, sequencerPos, h);
	ofSetLineWidth(1.0);

	glPopMatrix();
}
