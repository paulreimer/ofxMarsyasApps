#include "Mixer.h"

#define FADER_FIDUCIAL_ID 1

template <int id>
bool fiducialById(ofxFiducial &fiducial)
{
	return fiducial.getId() == id;
}

//--------------------------------------------------------------
Mixer::Mixer(string name)
: ofxMarsyasNetwork(name)
{
	priority = 2;

	disableAllEvents();
	ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
	ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);
	
	channels = mng.create("Fanout", "channels");
	registerSourceTypeAsChannelType("AudioSource", "MicrophoneChannel");
	registerSourceTypeAsChannelType("SoundFileSource", "SoundFileChannel");
}

//--------------------------------------------------------------
void
Mixer::setup()
{
	addMarSystem(channels);
	addMarSystem(mng.create("MixToMono", "mixdown"));
	addMarSystem(mng.create("AudioSink", "dest"));

	updctrl("mrs_natural/inSamples",		64);

	updctrl("AudioSink/dest/mrs_bool/initAudio",		true);
	updctrl("AudioSink/dest/mrs_natural/bufferSize",	1024);	

	run();
}

//--------------------------------------------------------------
void
Mixer::registerSourceTypeAsChannelType(string SourceSystem, string PrototypeSystem)
{
	bool oscbank_ = true;
	
	Marsyas::MarSystem *channel;
	Marsyas::MarSystem *gain, *source, *vocoder;
	
	channel = mng.create("Series", PrototypeSystem);
	source = mng.create(SourceSystem, "src");
	gain = mng.create("Gain", "gain");

	if (oscbank_)
		vocoder = mng.create("PhaseVocoderOscBank", "pvoc");
	else
		vocoder = mng.create("PhaseVocoder", "pvoc");

	channel->addMarSystem(source);
	channel->addMarSystem(gain);
	channel->addMarSystem(vocoder);

	gain->updctrl("mrs_real/gain",					0.0);

	vocoder->updctrl("mrs_natural/winSize",			512);
	vocoder->updctrl("mrs_natural/FFTSize",			512);
	vocoder->updctrl("mrs_natural/Interpolation",	64);
	vocoder->updctrl("mrs_natural/Decimation",		64);
	vocoder->updctrl("mrs_natural/Sinusoids",		80);
	vocoder->updctrl("mrs_string/convertMode",		"sorted");
	vocoder->updctrl("mrs_real/PitchShift",			1.0);
	
	vocoder->updctrl("mrs_bool/phaselock",			true);
	
	channel->linkctrl("mrs_real/gain", "Gain/gain/mrs_real/gain");
	channel->linkctrl("mrs_real/pitch", vocoder->getAbsPath()+"mrs_real/PitchShift");

	if (SourceSystem=="AudioSource")
	{
		channel->linkctrl("mrs_real/israte",
						  SourceSystem+"/src/mrs_real/israte");
		channel->linkctrl("mrs_bool/initAudio",
						  SourceSystem+"/src/mrs_bool/initAudio");
	}
	else if (SourceSystem=="SoundFileSource")
	{
		channel->linkctrl("mrs_string/filename",
						  SourceSystem+"/src/mrs_string/filename");
		source->updctrl("mrs_real/repetitions", -1.0);
	}
	
	mng.registerPrototype(PrototypeSystem, channel);
}

//--------------------------------------------------------------
Marsyas::MarSystem*
Mixer::addChannel(int id, string inAudioFileName)
{
	MarSystem* channel = NULL;

	if (inAudioFileName=="")
	{
		channel = mng.create("MicrophoneChannel",	ofToString(id));
		channel->updctrl("mrs_real/israte",			44100.0);
		channel->updctrl("mrs_bool/initAudio",		true);
	}
	else {
		channel = mng.create("SoundFileChannel",	ofToString(id));
		channel->updctrl("mrs_string/filename",		inAudioFileName);
	}

	channels->addMarSystem(channel);
	
	return channel;
}

//--------------------------------------------------------------
void
Mixer::update()
{
	if (fiducials==NULL)
		return;

	list<ofxFiducial>::iterator channel_fiducial, fader_fiducial;
	
	fader_fiducial = find_if(fiducials->begin(), fiducials->end(), fiducialById<FADER_FIDUCIAL_ID>);

	Marsyas::MarSystem* channel;
	string query;
	double dist, gain;
	double angle, pitch;
	//		double max_dist = VIDEO_WIDTH - (2*fiducial->getRootSize());
	double scale, units, max_units=4;
	
	for (channel_fiducial = fiducials->begin(); channel_fiducial != fiducials->end(); channel_fiducial++)
	{
		if (channel_fiducial->getId() == FADER_FIDUCIAL_ID)
			continue;
		
		// Start searching for SoundFileChannel
		query = "SoundFileChannel/"+ofToString(channel_fiducial->getId());
		channel = channels->getChildMarSystem(query);
		
		// Try searching for MicrophoneChannel
		if (channel == NULL)
		{
			query = "MicrophoneChannel/"+ofToString(channel_fiducial->getId());
			channel = channels->getChildMarSystem(query);
		}
		
		if (channel!=NULL)
		{
			// Update the pitch via fiducial rotation
			angle = channel_fiducial->getAngle();
			pitch = ofMap(angle, 0, TWO_PI, 0.0, 2.0);

			channel->updctrl("mrs_real/pitch", pitch);				

			// Update the gain via fiducial distance
			scale = channel_fiducial->getRootSize() / fader_fiducial->getRootSize();
			dist = channel_fiducial->getDistance(fader_fiducial->getX(), fader_fiducial->getY());
			units = dist * scale;
			gain = ofMap(max_units - units, 0, max_units, 0.0, 1.0, true);

			channel->updctrl("mrs_real/gain", gain);
		}
	}
}

//--------------------------------------------------------------
void
Mixer::draw()
{}
