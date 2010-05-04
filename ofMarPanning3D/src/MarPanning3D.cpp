#include "MarPanning3D.h"

#define	DEFAULT_SPECTRUM_LINES		50

using namespace Marsyas;

MarPanning3D::MarPanning3D()
{
//	targetRate = rate = sampleRate / bufferSize;
	inAudioFileName = ofToDataPath("wav/french_horn/6.wav");
//	targetRate = 100; // [10ms]
	targetRate = 1000;
}

MarPanning3D::~MarPanning3D()
{
	destroy();
}

void MarPanning3D::setup()
{
	// Initialize member variables
	rot.x					= 30;
	rot.y					= 0;
	rot.z					= 0;
	
	trans.x					= 0;
	// For 200
	//   trans.z = -127;
	//   trans.y = -5.5;
	
	// For 50
	//   trans.z = -52;
	//   trans.z = -1 * z_start;
	trans.z					= -70;
	trans.y					= -6.7;
	
	insamples				= 512;
	spectrum_bins			= insamples / 2.0;
	stereo_spectrum_bins	= insamples / 2.0;
	
	// Create space for the vertices we will display
	powerspectrum_ring_buffer = new double*[MAX_Z];
	panning_spectrum_ring_buffer = new double*[MAX_Z];
	
	for (int i = 0; i < MAX_Z; i++)
	{
		powerspectrum_ring_buffer[i] = new double[MAX_SPECTRUM_BINS];
		panning_spectrum_ring_buffer[i] = new double[MAX_STEREO_SPECTRUM_BINS];
	}
	clearRingBuffers();
	ring_buffer_pos = 0;
	
	//
	// Create the MarSystem
	//
	
	addMarSystem(mng.create("SoundFileSource", "src"));
	addMarSystem(mng.create("AudioSink", "dest"));
	
	MarSystem* fanout = mng.create("Fanout", "fanout");
	
    MarSystem* powerspectrum_series = mng.create("Series", "powerspectrum_series");
    powerspectrum_series->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
    powerspectrum_series->addMarSystem(mng.create("Windowing", "ham"));
    powerspectrum_series->addMarSystem(mng.create("Spectrum", "spk"));
    powerspectrum_series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
	powerspectrum_series->addMarSystem(mng.create("Gain", "gain"));
	
	MarSystem* stereobranches_series = mng.create("Series", "stereobranches_series");
	MarSystem* stereobranches_parallel = mng.create("Parallel", "stereobranches_parallel");
	MarSystem* left = mng.create("Series", "left");
	MarSystem* right = mng.create("Series", "right");
	
	left->addMarSystem(mng.create("Windowing", "hamleft"));
	left->addMarSystem(mng.create("Spectrum", "spkleft"));
	
	right->addMarSystem(mng.create("Windowing", "hamright"));
	right->addMarSystem(mng.create("Spectrum", "spkright"));
	
	stereobranches_parallel->addMarSystem(left);
	stereobranches_parallel->addMarSystem(right);
	stereobranches_series->addMarSystem(stereobranches_parallel);
	stereobranches_series->addMarSystem(mng.create("StereoSpectrum", "sspk"));
	
	stereobranches_series->addMarSystem(mng.create("Gain", "gain"));
	
	fanout->addMarSystem(powerspectrum_series);
	fanout->addMarSystem(stereobranches_series);
	
	addMarSystem(fanout);
	addMarSystem(mng.create("Gain", "gain"));
	
	updctrl("mrs_real/israte",								44100.0);
	updctrl("SoundFileSource/src/mrs_real/israte",			44100.0);
	updctrl("SoundFileSource/src/mrs_real/osrate",			44100.0);
	updctrl("AudioSink/dest/mrs_real/israte",				44100.0);
	updctrl("SoundFileSource/src/mrs_natural/inSamples",	insamples);
	updctrl("mrs_natural/inSamples",						insamples);
	
	if (inAudioFileName != "")
		updctrl("SoundFileSource/src/mrs_string/filename",	inAudioFileName);

	updctrl("SoundFileSource/src/mrs_real/repetitions",		-1.0);
	
	updctrl("mrs_natural/inSamples",						insamples);
	
	updctrl("mrs_real/israte",								44100.0);
	updctrl("AudioSink/dest/mrs_bool/initAudio",			true);
	
	updctrl("SoundFileSource/src/mrs_natural/pos",			0);

	if (inAudioFileName != "")
		play_state = true;
	
	// Create some handy pointers to access the MarSystem
	posPtr_		= getctrl("SoundFileSource/src/mrs_natural/pos");
	sizePtr_	= getctrl("SoundFileSource/src/mrs_natural/size");
	osratePtr_	= getctrl("SoundFileSource/src/mrs_real/osrate");
	initPtr_	= getctrl("AudioSink/dest/mrs_bool/initAudio");
	fnamePtr_	= getctrl("SoundFileSource/src/mrs_string/filename");
	
	initializeGL();
	run();
}

void MarPanning3D::destroy()
{}

void MarPanning3D::update()
{
	if (play_state)
		addDataToRingBuffer();
}

void MarPanning3D::draw()
{
	float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);
	
	float min_x = -1.5;
	float max_x = 1.5;
	float min_y = -1.5;
	float max_y = 1.5;

	float min_z = 3;
	float max_z = 30;
	
	glBegin(GL_LINES);
	glVertex3f(min_x,min_y,min_z);
	glVertex3f(min_x,min_y,max_z);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3f(min_x,max_y,min_z);
	glVertex3f(min_x,max_y,max_z);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3f(max_x,min_y,min_z);
	glVertex3f(max_x,min_y,max_z);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3f(max_x,max_y,min_z);
	glVertex3f(max_x,max_y,max_z);
	glEnd();
	
	double x;
	double y;
	double z;
	double size;
	
	for (int i = 0; i < MAX_Z; i++)
	{
		for (int j = 0; j < stereo_spectrum_bins; j++)
		{
			x = panning_spectrum_ring_buffer[(i + ring_buffer_pos) % MAX_Z][j];
			y = (log10(((22050.0 / double(spectrum_bins)) * j) + (0.5 * (22050.0 / double(spectrum_bins)))));
			z = i;

			x = x*50000;
//			y = (y-7.1);
			y = ofMap(y, -7,	14,		-50,	50);
			z = ofMap(z, 0.,	MAX_Z,	3,		30);

//			cout << "p at (" << x << "," << y << "," << z << ")" << endl;
			size = (powerspectrum_ring_buffer[(i + ring_buffer_pos) % MAX_Z][j]) * 2000;
			
			if (size > 0.5)
				size = 0.5;

			if (size > magnitude_cutoff)
			{
				glTranslated(x,y,z);
				float mcolor[3];

				// Red dots if big magnitude
				if (size > 0.4)
				{
					mcolor[0] = 1.0f;
					mcolor[1] = 0.0f;
					mcolor[2] = 0.0f;
					mcolor[3] = 1.0f;
				} else {
					mcolor[0] = (size*5);
					mcolor[1] = 1.0f;
					mcolor[2] = 0.0f;
					mcolor[3] = 1.0f;
				}
				
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);
				
				ofCircle(0, 0, size/10);
				glTranslated(-x,-y,-z);
			}
		}
	}
}

// Initialize the GL widget
void MarPanning3D::initializeGL()
{
	// Enable fog for depth cueing
	GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};
	//   GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);  // Fog colour of black (0,0,0)
	
	glFogfv(GL_FOG_COLOR, fogColor);    // Set fog color
	glFogi(GL_FOG_MODE, GL_LINEAR);       // Set the fog mode
	glFogf(GL_FOG_DENSITY, 0.5f);      // How dense will the fog be
	glHint(GL_FOG_HINT, GL_NICEST);     // Fog hint value : GL_DONT_CARE, GL_NICEST
	glEnable(GL_FOG);                   // Enable fog
	
	glFogf(GL_FOG_START, -94);          // Fog Start Depth
	glFogf(GL_FOG_END, -117);          // Fog End Depth
	
	// Antialias lines
	glEnable(GL_LINE_SMOOTH);
	glHint (GL_LINE_SMOOTH_HINT,GL_NICEST);
	
	GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void MarPanning3D::addDataToRingBuffer()
{
	mrs_realvec powerspectrum_data = getctrl("Fanout/fanout/Series/powerspectrum_series/PowerSpectrum/pspk/mrs_realvec/processedData")->to<mrs_realvec>();
	mrs_realvec panning_data = getctrl("Fanout/fanout/Series/stereobranches_series/StereoSpectrum/sspk/mrs_realvec/processedData")->to<mrs_realvec>();
	
	int powerspectrum_rows = powerspectrum_data.getRows();
	int panning_rows = panning_data.getRows();
	
	for (int i = 0; i < powerspectrum_rows; i++)
		powerspectrum_ring_buffer[ring_buffer_pos][i] = powerspectrum_data(i,0);
	
	for (int i = 0; i < panning_rows; i++)
		panning_spectrum_ring_buffer[ring_buffer_pos][i] = panning_data(i,0);
	
	ring_buffer_pos += 1;
	
	if (ring_buffer_pos >= MAX_Z)
		ring_buffer_pos = 0;
}

void MarPanning3D::setFFTBins(int val)
{
	if (val <= 10)
		set_fft_size(32 << val);
}

void MarPanning3D::set_fft_size(int val)
{
	setInSamples(val);
}

void MarPanning3D::playPause() 
{
	play_state = !play_state;
}

void MarPanning3D::setMagnitudeCutoff(int v)
{
	magnitude_cutoff = ((v*v*v) * 0.0001) / 10000;
}

void MarPanning3D::setSongPosition(int v)
{
	float fsize = sizePtr_->to<mrs_natural>();
	fsize *= v;
	
	int size = (int)fsize;
	updctrl(posPtr_, size);	
}

void MarPanning3D::clearRingBuffers()
{
	for (int i = 0; i < MAX_Z; i++)
	{
		for (int j = 0; j < spectrum_bins; j++)
			powerspectrum_ring_buffer[i][j] = 0.0;

		for (int j = 0; j < stereo_spectrum_bins; j++)
			panning_spectrum_ring_buffer[i][j] = 0.0;
	}
}

void MarPanning3D::setInSamples(int v)
{
	insamples = v;
	spectrum_bins = insamples / 2.0;
	stereo_spectrum_bins = insamples / 2.0;
	
	updctrl("mrs_natural/inSamples",insamples);
    clearRingBuffers();
}
