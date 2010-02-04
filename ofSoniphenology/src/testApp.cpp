#include "testApp.h"
/*
#define MAP_UNITS_TO_LAT_DEGREES	2.3*MAP_LAT_RANGE/7.65217
#define MAP_UNITS_TO_LONG_DEGREES	2.3*MAP_LONG_RANGE/8.69565 
*/

struct fiducialById:
public std::unary_function <ofxFiducial, bool>{
	int id;
	fiducialById(int id): id(id) {}
	bool operator () (ofxFiducial& fiducial) const
	{
		return fiducial.getId() == id;
	}
};

//--------------------------------------------------------------
void
testApp::setup()
{
//	ofSoundStreamListDevices();
	
	ofSetBackgroundAuto(true);
	ofBackground(0,0,0);

	bg.loadImage("chrome/bg-dark.png");
	bg.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);

	ofSetFrameRate(VIDEO_FPS);

	glutSetWindowTitle("A tangible interface for sonification of geo-spatial and phenological data at multiple time-scales.");
	
//	font.loadFont(ofToDataPath("frabk.ttf"), 14);
	font.loadFont(ofToDataPath("HelveticaBold.ttf"), 12);
	font_sm.loadFont(ofToDataPath("Helvetica.ttf"), 9);

//	cvGrabber.listDevices();
	cvGrabber.setDesiredFrameRate(VIDEO_FPS);
	cvGrabber.initGrabber(VIDEO_SIZE);
	videoSize.set(VIDEO_SIZE);
	
	imRGB.allocate(VIDEO_SIZE);
	imBW.allocate(VIDEO_SIZE);

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
	soundEngine.instruments[1*2] = "wav/bass";
	soundEngine.instruments[2*2] = "wav/clarinet";
	soundEngine.instruments[3*2] = "wav/english_horn";
	soundEngine.instruments[4*2] = "wav/flute";
	soundEngine.instruments[5*2] = "wav/french_horn";
	soundEngine.instruments[6*2] = "wav/harpsichord";
	soundEngine.instruments[7*2] = "wav/marimba";
	soundEngine.instruments[8*2] = "wav/piano";
	soundEngine.instruments[9*2] = "wav/tuba";

#ifdef USE_GEO_DATA
	soundEngine.geoData = &geoData;
#endif
#endif

#ifdef USE_GUI
#ifdef USE_FIDUCIAL_TRACKER
	gui.addPage("Fiducials");
	gui.addContent("RGB",		imRGB);
	gui.addContent("BW",		imBW);
	gui.addContent("Threshold",	fiducials.imThreshold, VIDEO_WIDTH);
#ifdef PREFER_OPENCV
	gui.addSlider("Block Size",	fiducials.blocksize, 0, 49);
	gui.addSlider("Offset",		fiducials.offset, 0, 100);
	gui.addToggle("Gauss",		fiducials.gauss);
	gui.addToggle("Invert",		fiducials.invert);
#else
	gui.addSlider("Threshold",	fiducials.threshold, 0, 255);
#endif
	gui.addContent("Fiducials",	fiducials);
#endif
//	gui.setup();
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
	cvGrabber.grabFrame();
#ifdef USE_CAMERA
	cameraGrabber.grabFrame();
#endif

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
	bg.draw(0,0, ofGetWidth(), ofGetHeight());
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

#ifdef USE_CAMERA
	cameraImage.draw(ofGetWidth() - (CAMERA_WIDTH/1.5) - 10,
					 ofGetHeight() - (CAMERA_HEIGHT/1.5) - 10,
					 CAMERA_WIDTH/1.5,
					 CAMERA_HEIGHT/1.5);
#endif	
	
#ifdef USE_GUI
	gui.draw();
#endif
	
#ifdef USE_FIDUCIAL_TRACKER
	glPushMatrix();
//	glTranslatef(x, y, 0.);
	glScalef(ofGetWidth()/fiducials.getWidth(),
			 ofGetHeight()/fiducials.getHeight(), 1.);

	for (list<ofxFiducial>::iterator fiducial = fiducials.tracker.fiducialsList.begin();
		 fiducial != fiducials.tracker.fiducialsList.end();
		 fiducial++)
	{
		ofSetColor(0x00ff00);
		fiducial->draw(0,0);
		ofSetColor(0x0000ff);
		fiducial->drawCorners(0,0);
	}

	glPopMatrix();
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
/*	
#ifdef USE_GEO_DATA
	ofPoint nwCorner(-90, 0);
	ofPoint seCorner(90, 180);
	ofPoint timeInterval(1905, 2010);

	geoData.query(1, nwCorner, seCorner, timeInterval);
#endif
*/
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

	if (map_fiducial != fiducials.end() && VALID_FIDUCIAL(map_fiducial))
	{
		map_ref.set(map_fiducial->getX(), map_fiducial->getY());
		map_angle = map_fiducial->getAngle();
		map_rsize = map_fiducial->getRootSize();
	}
	
	for (from_fiducial = fiducials.begin(); from_fiducial != fiducials.end(); from_fiducial++)
	{
		if (from_fiducial->getId() == FADER_FIDUCIAL_ID
			|| EVEN(from_fiducial->getId())
			|| !VALID_FIDUCIAL(from_fiducial))
			continue;

		to_fiducial = find_if(fiducials.begin(), fiducials.end(), fiducialById(from_fiducial->getId()+1));

		if (to_fiducial == fiducials.end()
			|| !VALID_FIDUCIAL(to_fiducial))
			continue;

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

		from.set(from_ref.x, from_ref.y + QUERY_POINT_OFFSET*(from_fiducial->getRootSize()/map_rsize));
		to.set(to_ref.x, to_ref.y + QUERY_POINT_OFFSET*(to_fiducial->getRootSize()/map_rsize));

		from.rotateRad(from_fiducial->getAngle(), from_ref);
		to.rotateRad(to_fiducial->getAngle(), to_ref);

#ifdef USE_GEO_DATA
		ofPoint timeInterval((int)ofMap(from_fiducial->getAngle(),	0, TWO_PI, MIN_TIMESTAMP_YEAR, MAX_TIMESTAMP_YEAR),
							 (int)ofMap(to_fiducial->getAngle(),	0, TWO_PI, MIN_TIMESTAMP_YEAR, MAX_TIMESTAMP_YEAR));

		if (timeInterval.y > timeInterval.x)
			geoData.query(from_fiducial->getId(), from, to, timeInterval);
		else {
			cout<< "Attempted invalid query. "
				<< "Paul suspects you have something in mind when you are trying this." << endl;
			// is this what you had in mind?
//			swap(timeInterval.x, timeInterval.y);
//			geoData.query(from_fiducial->getId(), from, to, timeInterval);
		}
#endif		
//		cout << "Query from: (" << from.x			<< ","		<< from.y	<< ")"	<< endl;
//		cout << "      to:   ("	<< to.x				<< ","		<< to.y		<< ")"	<< endl;
//		cout << "between "		<< timeInterval.x	<< " and "	<< timeInterval.y	<< endl;
	}
}
#endif
