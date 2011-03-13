#include "testApp.h"
/*
#define MAP_UNITS_TO_LAT_DEGREES	2.3*MAP_LAT_RANGE/7.65217
#define MAP_UNITS_TO_LONG_DEGREES	2.3*MAP_LONG_RANGE/8.69565 
*/

#ifdef USE_FIDUCIAL_TRACKER
struct fiducialById:
public std::unary_function <ofxFiducial, bool>{
	int id;
	fiducialById(int id): id(id) {}
	bool operator () (ofxFiducial& fiducial) const
	{
		return fiducial.getId() == id;
	}
};
#endif

//--------------------------------------------------------------
void
testApp::setup()
{
//	ofSoundStreamListDevices();
	
	ofSetBackgroundAuto(true);
	ofBackground(0,0,0);

	bg.loadImage("chrome/bg-dark.png");
	bg.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);

#ifdef USE_CV	
	ofSetFrameRate(VIDEO_FPS);
#endif

	glutSetWindowTitle("A tangible interface for sonification of geo-spatial and phenological data at multiple time-scales.");
	
	font.loadFont("fonts/HelveticaBold.ttf", 12);
	font_sm.loadFont("fonts/Helvetica.ttf", 9);

#ifdef USE_CV	
	cvGrabber.setDesiredFrameRate(VIDEO_FPS);
	cvGrabber.initGrabber(VIDEO_SIZE);
	videoSize.set(VIDEO_SIZE);
	
	imRGB.allocate(VIDEO_SIZE);
	imBW.allocate(VIDEO_SIZE);
#endif
	
#ifdef USE_CAMERA
	cameraGrabber.setDesiredFrameRate(24);
	cameraGrabber.initGrabber(CAMERA_SIZE);
	cameraImage.allocate(CAMERA_SIZE, OF_IMAGE_COLOR);	
#endif

#ifdef USE_FIDUCIAL_TRACKER	
	fiducials.videoSize.x = videoSize.x;
	fiducials.videoSize.y = videoSize.y;

	fiducials.imBW = &imBW;

	fiducials.setup();

	ofAddListener(fiducials.newFrame, this, &testApp::processFiducials);
#endif
	
#ifdef USE_SONIFICATION_ENGINE
	soundEngine.instruments[4*2] = "wav/flute";
	soundEngine.instruments[1*2] = "wav/clarinet";
//	soundEngine.instruments[3*2] = "wav/marimba";
//	soundEngine.instruments[4*2] = "wav/flute";
	soundEngine.instruments[5*2] = "wav/french_horn";
//	soundEngine.instruments[6*2] = "wav/bass";
//	soundEngine.instruments[3*2] = "wav/english_horn";
//	soundEngine.instruments[8*2] = "wav/piano";
//	soundEngine.instruments[9*2] = "wav/tuba";

#ifdef USE_GEO_DATA
	soundEngine.geoData = &geoData;
#endif
#endif

#ifdef USE_GUI
	gui.setup();
<<<<<<< HEAD:ofSoniphenology/src/testApp.cpp
	defaultSimpleGuiConfig.gridSize.x = 200;
	defaultSimpleGuiConfig.gridSize.y = 32;
=======
>>>>>>> 14af018... Added USE_CV ifdefs, random queries to GeoData on mouse click:ofSoniphenology/src/testApp.cpp
#ifdef USE_FIDUCIAL_TRACKER
	gui.addPage("Fiducials");
	gui.addContent("RGB",		imRGB, 180);
	gui.addContent("BW",		imBW, 180);
	gui.addContent("Threshold",	fiducials.imThreshold, 180);
	gui.addContent("Fiducials",	fiducials, 180);
#ifdef PREFER_OPENCV
	gui.addSlider("Block Size",	fiducials.blocksize, 0, 49);
	gui.addSlider("Offset",		fiducials.offset, 0, 100);
//	gui.addToggle("Gauss",		fiducials.gauss);
//	gui.addToggle("Invert",		fiducials.invert);
#else
	gui.addSlider("Threshold",	fiducials.threshold, 0, 255);
#endif
	gui.addSlider("Tempo",		soundEngine.tempo, 1, 50);
//	gui.addSlider("Smoothing",	soundEngine.lerpFactor, 0., 1.);
#endif
	
#ifdef USE_CAMERA
//	gui.addContent("Camera View", cameraImage, 320);
#endif
#endif

#ifdef USE_GEO_DATA
	geoData.setup();
#endif
	
	map_ref		.set(0,0);
	map_angle	= 0;
	map_rsize	= 1;
}

//--------------------------------------------------------------
void
testApp::update()
{
#ifdef USE_CV
	cvGrabber.grabFrame();
#endif
#ifdef USE_CAMERA
	cameraGrabber.grabFrame();
#endif

#ifdef USE_CV
	if (cvGrabber.isFrameNew())
	{
#ifdef USE_OPENCV
		imRGB.setFromPixels(cvGrabber.getPixels(), videoSize.x, videoSize.y);
		imBW = imRGB;
#endif

#ifdef USE_FIDUCIAL_TRACKER
		fiducials.update();
#endif
	}
<<<<<<< HEAD:ofSoniphenology/src/testApp.cpp
=======
#endif
>>>>>>> 14af018... Added USE_CV ifdefs, random queries to GeoData on mouse click:ofSoniphenology/src/testApp.cpp

#ifdef USE_CAMERA
	if (cameraGrabber.isFrameNew())
		cameraImage.setFromPixels(cameraGrabber.getPixels(), CAMERA_SIZE, OF_IMAGE_COLOR);
#endif
}

//--------------------------------------------------------------
void
testApp::draw()
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//	bg.draw(0,0, ofGetWidth(), ofGetHeight());
	cameraImage.draw(0,0, ofGetWidth(),ofGetHeight());
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	float xpos = 1 + defaultSimpleGuiConfig.gridSize.x + defaultSimpleGuiConfig.offset.x - defaultSimpleGuiConfig.padding.x;
	soundEngine.draw(xpos,ofGetHeight()-120, ofGetWidth()-xpos,100);
#ifdef USE_GUI
	gui.draw();
#endif
}

//--------------------------------------------------------------
void
testApp::keyPressed(int key)
{}

//--------------------------------------------------------------
void
testApp::keyReleased(int key)
{}

//--------------------------------------------------------------
void
testApp::mouseMoved(int x, int y)
{}

//--------------------------------------------------------------
void
testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::mousePressed(int x, int y, int button)
{
<<<<<<< HEAD:ofSoniphenology/src/testApp.cpp
/*	
	static int instr_idx;
#ifdef USE_GEO_DATA
	ofPoint nwCorner(-90, 0);
	ofPoint seCorner(90, 180);
	ofPoint timeInterval(1905, 2010);

	instr_idx +=2;
	geoData.query(instr_idx, nwCorner, seCorner, timeInterval);
=======
#ifdef USE_GEO_DATA
	float bounds = 1/4.0;
	ofPoint nwCorner(ofRandom(MAP_NW_CORNER_LAT,
							  MAP_NW_CORNER_LAT + (MAP_RANGE_LAT*bounds)),
					 ofRandom(MAP_NW_CORNER_LONG,
							  MAP_SE_CORNER_LONG - (MAP_RANGE_LONG*bounds)));
	ofPoint seCorner(ofRandom(MAP_NW_CORNER_LAT + (MAP_RANGE_LAT*bounds),
							  MAP_SE_CORNER_LAT),
					 ofRandom(MAP_SE_CORNER_LONG - (MAP_RANGE_LONG*bounds),
							  MAP_SE_CORNER_LONG));
	ofPoint timeInterval(ofRandom(MIN_TIMESTAMP_YEAR,
								  MIN_TIMESTAMP_YEAR + (TIMESTAMP_RANGE_YEARS*bounds)),
						 ofRandom(MAX_TIMESTAMP_YEAR - (TIMESTAMP_RANGE_YEARS*bounds),
								  MAX_TIMESTAMP_YEAR));
	
	if (timeInterval.x > timeInterval.y)
		swap(timeInterval.x, timeInterval.y);
>>>>>>> 14af018... Added USE_CV ifdefs, random queries to GeoData on mouse click:ofSoniphenology/src/testApp.cpp
#endif
}

//--------------------------------------------------------------
void
testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::windowResized(int w, int h)
{}

#ifdef USE_FIDUCIAL_TRACKER
//--------------------------------------------------------------
void
testApp::processFiducials(list<ofxFiducial>& fiducials)
{	
	list<ofxFiducial>::iterator from_fiducial, to_fiducial, fader_fiducial, map_fiducial;
	
	fader_fiducial = find_if(fiducials.begin(), fiducials.end(), fiducialById(FADER_FIDUCIAL_ID));
	map_fiducial = find_if(fiducials.begin(), fiducials.end(), fiducialById(MAP_FIDUCIAL_ID));

	int channel;
	ofxVec2f from, to;
	ofxVec2f from_ref, to_ref;
	double from_angle, to_angle;

	if (map_fiducial != fiducials.end() && VALID_FIDUCIAL(map_fiducial))
	{
		map_ref.set(map_fiducial->getX(), map_fiducial->getY());
		map_angle = map_fiducial->getAngle();
		map_rsize = map_fiducial->getRootSize();
	}

//	geoData.clear();
	for (from_fiducial = fiducials.begin(); from_fiducial != fiducials.end(); from_fiducial++)
	{
		if (from_fiducial->getId() == FADER_FIDUCIAL_ID
			|| EVEN(from_fiducial->getId()))
			continue;

		to_fiducial = find_if(fiducials.begin(), fiducials.end(), fiducialById(from_fiducial->getId()+1));

		if (to_fiducial == fiducials.end())
			continue;

		
//		if (!VALID_FIDUCIAL(to_fiducial) || !VALID_FIDUCIAL(from_fiducial))
		if (to_fiducial->life <= 10 || to_fiducial->life <= 10)
		{
#ifdef USE_GEO_DATA
			geoData.unquery(from_fiducial->getId());
#endif		
			continue;
		}

		from_angle	= from_fiducial->getAngle()	- map_angle;
		to_angle	= to_fiducial->getAngle()	- map_angle;

		// Locate the fiducials centers
		from_ref.set((from_fiducial->getX()	- map_ref.x)/map_rsize,
					 (from_fiducial->getY()	- map_ref.y)/map_rsize);
		to_ref.set	((to_fiducial->getX()	- map_ref.x)/map_rsize,
					 (to_fiducial->getY()	- map_ref.y)/map_rsize);

		from_ref.rotateRad(-map_angle);
		to_ref.rotateRad(-map_angle);

		from_ref.set(	ofMap(from_ref.x,	0.0, MAP_UNITS_LAT,	MAP_NW_CORNER_LAT,	MAP_SE_CORNER_LAT),
						ofMap(from_ref.x,	0.0, MAP_UNITS_LONG,MAP_NW_CORNER_LONG, MAP_SE_CORNER_LONG));
		to_ref.set(		ofMap(to_ref.x,		0.0, MAP_UNITS_LAT,	MAP_NW_CORNER_LAT,	MAP_SE_CORNER_LAT),
						ofMap(to_ref.x,		0.0, MAP_UNITS_LONG,MAP_NW_CORNER_LONG,	MAP_SE_CORNER_LONG));

//		cout << "from_ref @ (" << from_ref.x << "," << from_ref.y << ")" << endl;
//		cout << "to_ref @ (" << to_ref.x << "," << to_ref.y << ")" << endl;
/*
		from.set(from_ref.x, from_ref.y + QUERY_POINT_OFFSET*(from_fiducial->getRootSize()/map_rsize));
		to.set(to_ref.x, to_ref.y + QUERY_POINT_OFFSET*(to_fiducial->getRootSize()/map_rsize));

		from.rotateRad(from_angle, from_ref);
		to.rotateRad(to_angle, to_ref);
*/
		from = from_ref;
		to = to_ref;
		
		if (from_angle < 0)
			from_angle += TWO_PI;

		if (to_angle < 0)
			to_angle += TWO_PI;

		ofPoint timeInterval((int)ofMap(from_angle,	0, TWO_PI, MIN_TIMESTAMP_YEAR, MAX_TIMESTAMP_YEAR),
							 (int)ofMap(to_angle,	0, TWO_PI, MIN_TIMESTAMP_YEAR, MAX_TIMESTAMP_YEAR));

		if (timeInterval.x > timeInterval.y)
			swap(timeInterval.x, timeInterval.y);
		
#ifdef USE_GEO_DATA
		geoData.query(from_fiducial->getId(), from, to, timeInterval);
#endif
	}
}
#endif
