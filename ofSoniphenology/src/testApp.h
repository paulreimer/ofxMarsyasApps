#pragma once

#include "ofMain.h"
#include "settings.h"

#include "ofxMSAInteractiveObject.h"

#ifdef USE_FIDUCIAL_TRACKER
#include "ofxFiducialTracker.h"
#include "FiducialTracker.h"
#endif

#ifdef USE_SONIFICATION_ENGINE
#include "SonificationEngine.h"
#endif

#ifdef USE_GEO_DATA
#include "GeoData.h"
#endif

#ifdef USE_GUI
#include "ofxSimpleGuiToo.h"
#endif

#include "ofxVec2f.h"

class testApp
: public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

#ifdef USE_FIDUCIAL_TRACKER
	void processFiducials(list<ofxFiducial>& fiducialsList);
#endif

	ofImage			bg;

	ofVideoGrabber 	cvGrabber;
	ofPoint			videoSize;

	ofVideoGrabber 	cameraGrabber;
	ofImage			cameraImage;
	
	ofTrueTypeFont	font;
	ofTrueTypeFont	font_sm;

#ifdef USE_OPENCV
	ofxCvColorImage		imRGB;
	ofxCvGrayscaleImage	imBW;
#endif

#ifdef USE_GUI
	ofxSimpleGuiToo gui;
#endif
	
#ifdef USE_FIDUCIAL_TRACKER
	FiducialTracker	fiducials;	
#endif
	
#ifdef USE_SONIFICATION_ENGINE
	SonificationEngine soundEngine;
#endif

#ifdef USE_GEO_DATA
	GeoData geoData;
#endif

	ofxVec2f	map_ref;
	float		map_angle;
	float		map_rsize;
};
