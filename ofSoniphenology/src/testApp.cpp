#include "testApp.h"

#define EVEN(n) (bool)((int)n % 2) 
#define ODD(n) (!(bool)((int)n % 2))

#define MAP_NW_CORNER_LAT			48
#define MAP_NW_CORNER_LONG			121
#define MAP_UNITS_TO_LAT_DEGREES	2.3/10
#define MAP_UNITS_TO_LONG_DEGREES	2.3/10
#define QUERY_POINT_OFFSET			4/MAP_UNITS_TO_LONG_DEGREES
#define MIN_TIMESTAMP_YEAR			2000
#define MAX_TIMESTAMP_YEAR			2010
#define TIMESTAMP_RANGE_YEARS		MAX_TIMESTAMP_YEAR - MIN_TIMESTAMP_YEAR

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

//	soundEngine.responses = &geoData.getResponses();
	soundEngine.geoData = &geoData;

	ofAddListener(fiducials.newFrame, this, &testApp::processFiducials);
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

	for (int i=0; i<POIs.size(); i++)
	{
		ofDrawBitmapString(POIs[i].first, POIs[i].second.x, POIs[i].second.y);
		ofCircle(POIs[i].second.x, POIs[i].second.y, 5);
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
#ifdef USE_GEO_DATA
	ofPoint nwCorner(-90, 0);
	ofPoint seCorner(90, 180);
	ofPoint timeInterval(1905, 2010);

	geoData.query(1, nwCorner, seCorner, timeInterval);
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
	
	POIs.clear();

	fader_fiducial = find_if(fiducials.begin(), fiducials.end(), fiducialById(FADER_FIDUCIAL_ID));
	map_fiducial = find_if(fiducials.begin(), fiducials.end(), fiducialById(MAP_FIDUCIAL_ID));

	int channel;
	ofxVec2f from, to;
	ofxVec2f from_ref, to_ref;
	ofxVec2f epicentre;
	double angle;

	if (map_fiducial != fiducials.end())
	{
		map_ref.set(map_fiducial->getX(), map_fiducial->getY());
		map_angle = map_fiducial->getAngle();
		map_rsize = map_fiducial->getRootSize();
	}
	
	ofPoint scaleFac(1/(map_rsize*MAP_UNITS_TO_LAT_DEGREES),
					 1/(map_rsize*MAP_UNITS_TO_LONG_DEGREES));
	// new co-ordinate system, in units of root size of map fiducial
	// relative to center of map fiducial
	ofPoint origin(MAP_NW_CORNER_LAT,MAP_NW_CORNER_LONG);
	ofxVec2f vx(cosf(map_angle)*scaleFac.x, -sinf(map_angle)*scaleFac.x);
	ofxVec2f vy(sinf(map_angle)*scaleFac.y, cosf(map_angle)*scaleFac.y);

	for (from_fiducial = fiducials.begin(); from_fiducial != fiducials.end(); from_fiducial++)
	{
		if (from_fiducial->getId() == FADER_FIDUCIAL_ID || EVEN(from_fiducial->getId()))
			continue;

		to_fiducial = find_if(fiducials.begin(), fiducials.end(), fiducialById(from_fiducial->getId()+1));

		if (to_fiducial == fiducials.end())
			continue;

		// Locate the fiducials centers
		from_ref.set(from_fiducial->getX()	- map_ref.x,
					 from_fiducial->getY()	- map_ref.y);
		to_ref.set	(to_fiducial->getX()	- map_ref.x,
					 to_fiducial->getY()	- map_ref.y);

		from_ref.map(origin, vx, vy);
		to_ref.map(origin, vx, vy);
/*
		from_ref.rotateRad(-map_angle, map_ref);
		to_ref.rotateRad(-map_angle, map_ref);
*/		
		from.set(from_ref.x, from_ref.y + QUERY_POINT_OFFSET*(from_fiducial->getRootSize()/map_rsize));
		to.set(to_ref.x, to_ref.y + QUERY_POINT_OFFSET*(to_fiducial->getRootSize()/map_rsize));

		from.rotateRad(from_fiducial->getAngle(), from_ref);
		to.rotateRad(to_fiducial->getAngle(), to_ref);

		epicentre = from.getMiddle(to);
		angle = (from_ref-epicentre).angleRad(to_ref-epicentre);

#ifdef USE_GEO_DATA
		
		ofPoint timeInterval(MIN_TIMESTAMP_YEAR + TIMESTAMP_RANGE_YEARS * (from_fiducial->getAngle() / TWO_PI),
							 MIN_TIMESTAMP_YEAR + TIMESTAMP_RANGE_YEARS * (to_fiducial->getAngle() / TWO_PI));

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
		
		POIs.push_back(make_pair("from_" + ofToString(from_fiducial->getId()), from));
		POIs.push_back(make_pair("to_" + ofToString(to_fiducial->getId()), to));
		cout << "angle = " << angle << endl;
	}
}
#endif
