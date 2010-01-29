#include "PhaseVocoder.h"

//--------------------------------------------------------------
PhaseVocoder::PhaseVocoder(string name)
: ofxMarsyasNetwork(name)
{
//	inAudioFileName = ofToDataPath("bar.wav");
	inAudioFileName = "/tmp/gravity.wav";

	disableAllEvents();
	enableMouseEvents();
	ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
	ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);	
}

//--------------------------------------------------------------
void
PhaseVocoder::setup()
{
	fftSize = 512;
	windowSize = 512;
	D = 64;
	I = 64;
	sinusoids = 80;
	bufferSize = 1024;
	V = 1;
	oscbank_ = true;
	
	pitch = 1.0;
	microphone_ = false;
	convertmode_ = "sorted";
	
	if (microphone_)
		addMarSystem(mng.create("AudioSource", "src"));
	else
		addMarSystem(mng.create("SoundFileSource", "src"));
	
	if (oscbank_)
	{
		addMarSystem(mng.create("PhaseVocoderOscBank", "pvoc"));
		vocoderName = "PhaseVocoderOscBank/pvoc";
	}
	else {
		addMarSystem(mng.create("PhaseVocoder", "pvoc"));
		vocoderName = "PhaseVocoder/pvoc";
	}
	
	updctrl(vocoderName+"/mrs_natural/winSize",			windowSize);
	updctrl(vocoderName+"/mrs_natural/FFTSize",			fftSize);
	updctrl(vocoderName+"/mrs_natural/Interpolation",	I);
	updctrl(vocoderName+"/mrs_natural/Decimation",		D);
	updctrl(vocoderName+"/mrs_natural/Sinusoids",		sinusoids);
	updctrl(vocoderName+"/mrs_string/convertMode",		convertmode_);
	updctrl(vocoderName+"/mrs_real/PitchShift",			pitch);
	
	updctrl(vocoderName+"/mrs_bool/phaselock",			true);
		
	updctrl("AudioSink/dest/mrs_natural/bufferSize",	bufferSize);
		
	updctrl("mrs_natural/inSamples",					D);

	addMarSystem(mng.create("AudioSink", "dest"));

	if (microphone_) 
	{
		updctrl("AudioSource/src/mrs_real/israte", 44100.0);
		updctrl("AudioSource/src/mrs_bool/initAudio", true);
	}
	else 
	{
		updctrl("SoundFileSource/src/mrs_string/filename", inAudioFileName);
	}
	
	updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	run();
}

//--------------------------------------------------------------
void
PhaseVocoder::update()
{
	float currentPitch = getctrl(vocoderName+"/mrs_real/PitchShift")->to<Marsyas::mrs_real>();
	if (pitch != currentPitch)
		updctrl(vocoderName+"/mrs_real/PitchShift", pitch);
}

//--------------------------------------------------------------
void
PhaseVocoder::draw()
{}

//--------------------------------------------------------------
void
PhaseVocoder::mouseMoved(int x, int y)
{
//	pitch = (float)x / (float)ofGetWidth();
}
