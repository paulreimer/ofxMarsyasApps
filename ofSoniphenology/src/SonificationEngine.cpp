#include "SonificationEngine.h"

#define INSTRUMENT_NOTE(i,n) string("instrument"+ofToString(i)+"_note"+ofToString(n))

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

//--------------------------------------------------------------
SonificationEngine::SonificationEngine(string name)
: ofxMarsyasNetwork(name)
{
	nTicks		= 0;
	position	= 0;
	positionPrev= 0;
	tempo		= 10;

	priority	= 2;

	disableAllEvents();
	ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
	ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);
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

			source->updctrl("mrs_string/filename",	filename);
			source->updctrl("mrs_bool/mute",		true);
		}
	}

	addMarSystem(mng.create("MixToMono", "mono"));
	addMarSystem(mng.create("AudioSink", "dest"));
	updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	run();
}

//--------------------------------------------------------------
void
SonificationEngine::update()
{
	nTicks++;
	// Place to wrap around
	if (position % 90 == 0)
	{
		position = 0;
		// Change nTicks to change the tempo.  Bigger == slower
        if (nTicks % tempo == 0)
            position++;
	}

	if (geoData==NULL || geoData->responses.empty())
		return;

	map<int, GeoData::response_t>::iterator resp_iter;
	vector<string>::iterator date_iter;

//	while (!geoData->lock())
//	{}
	
	int year, month, day;
	int year_idx, date_idx;
	
	geoData->lock();
	resp_iter = geoData->responses.begin();
	while (resp_iter != geoData->responses.end())
	{
		int tag						= resp_iter->first;
		GeoData::response_t resp	= resp_iter->second;

		vector<string>& dates = resp_iter->second.dates;

		std::locale slasher(std::locale::classic(), new slash_skipper);
		stringstream strstrm;
		strstrm.imbue(slasher);

		cout << "Response to query " << tag << ", dates: " << resp.dates.size() << endl;
		for (date_iter = resp.dates.begin();
			 date_iter != resp.dates.end();
			 date_iter++)
		{
			cout << "Date: " << *date_iter << endl;

			strstrm.str(*date_iter);
			strstrm >> year >> month >> day;

			year_idx = year - MIN_TIMESTAMP_YEAR;
			date_idx = (month*30.5) + day - MIN_TIMESTAMP_DAYS;

			if (date_idx <= position && date_idx > positionPrev)
			{
				positionPrev = position;
				updctrl("mrs_bool/mute_"	+ INSTRUMENT_NOTE(tag,year_idx),	false);
				updctrl("mrs_natural/pos_"	+ INSTRUMENT_NOTE(tag,year_idx),	0);
			}
		}

		++resp_iter;
		geoData->responses.erase(tag);
	}
	geoData->unlock();
}

//--------------------------------------------------------------
void
SonificationEngine::draw()
{}
