#pragma once

#include "ofxMarsyasNetwork.h"

// sness
#define MAX_Z 50
#define MAX_SPECTRUM_BINS 65536
#define MAX_STEREO_SPECTRUM_BINS 65536

#define TIMER_COUNT_STEPS 100.0

class MarPanning3D
: public ofxMarsyasNetwork
{
public:
	MarPanning3D();
	~MarPanning3D();
	
	void allocate();
	void setup();
	void destroy();
	
	void update();
	void draw();

	string inAudioFileName;

	void setPos(int value);
	void setPos(); 
	
	void animate();

	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);
	
	void setXTranslation(int val);
	void setYTranslation(int val);
	void setZTranslation(int val);
	
	void setFogStart(int val);
	void setFogEnd(int val);
	
	void setFFTBins(int val);
	
	void setMagnitudeCutoff(int v);
	void setNumVertices(int v);
	void setDotSize(int v);
	void setSongPosition(int v);
	void setDisplaySpeed(int v);
	
	void playPause();

	// The current x,y,z rotation angles
	ofPoint rot;
	ofPoint trans;
	
	double fogStart;
	double fogEnd;	
	
	bool play_state;
	
protected:
	void initializeGL();                    // Initialize the GL window
	void clearRingBuffers();

private:
	void setWaveformData();

	void addDataToRingBuffer();

	void setInSamples(int);
	void set_fft_size(int);	


	// Windowed waveform for display
	Marsyas::mrs_realvec waveform_data;
	
	Marsyas::MarControlPtr posPtr_;
	Marsyas::MarControlPtr sizePtr_;
	Marsyas::MarControlPtr osratePtr_;
	Marsyas::MarControlPtr initPtr_;
	Marsyas::MarControlPtr fnamePtr_;	
	
	// A ring buffer that holds our data
	double **powerspectrum_ring_buffer;
	double **panning_spectrum_ring_buffer;
	int ring_buffer_pos; // The current head position in the ring buffer
	
	int stereo_spectrum_bins;
	int spectrum_bins;
	int insamples;
	float magnitude_cutoff;
};
