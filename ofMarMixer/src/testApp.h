#pragma once

#include "ofMain.h"
#include "settings.h"

#include "ofxMSAInteractiveObject.h"
#include "Mixer.h"

#ifdef USE_FIDUCIAL_TRACKER
#include "ofxFiducialTracker.h"
#include "FiducialTracker.h"
#include "GuiGraph.h"
#endif

#ifdef USE_SKIN_DETECTOR
#include "SkinDetector.h"
#endif

#ifdef USE_GUI
#include "ofxSimpleGuiToo.h"
#endif

#ifdef USE_CVD
#include "ofxCvdImage.h"
#endif

class testApp : public ofBaseApp
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

	ofImage	bg;

	Mixer mixer;

	ofVideoGrabber 		cvGrabber;
	ofxCvdColorImage	imRGB;
	ofPoint videoSize;
	ofPoint drawVideoSize;
	ofPoint scale;

	ofVideoGrabber 		cameraGrabber;
	ofImage				cameraImage;

	ofTrueTypeFont font;
	ofTrueTypeFont font_sm;

#ifdef USE_GUI
	ofxSimpleGuiToo gui;
#endif
	
#ifdef USE_FIDUCIAL_TRACKER
	vector<ofImage> fiducialImages;

	FiducialTracker	fiducials;	
	ofxFiducialBacked::gui::GuiGraph fiducial_gui;
#endif

#ifdef USE_SKIN_DETECTOR
	SkinDetector	skin;	
#endif
};
