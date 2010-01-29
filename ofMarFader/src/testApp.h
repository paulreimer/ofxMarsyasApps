#pragma once

#include "ofMain.h"
#include "ofxFiducialTracker.h"

#define	USE_CVD
#define	USE_FIDUCIAL_TRACKER
#undef	USE_SKIN_DETECTOR

#include "Mixer.h"
#include "FiducialTracker.h"
#include "SkinDetector.h"

#include "ofxMSAInteractiveObject.h"

#ifdef USE_CVD
#include <cvd/image.h>
#include <cvd/byte.h>
#include <cvd/rgb.h>
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

	Mixer mixer;

	ofVideoGrabber 		cvGrabber;
	CVD::Image<CVD::Rgb<CVD::byte> > imRGB;
	ofPoint videoSize;

	ofVideoGrabber 		cameraGrabber;
	ofImage				cameraImage;

	ofTrueTypeFont font;
	ofTrueTypeFont font_sm;
	
#ifdef USE_FIDUCIAL_TRACKER
	vector<ofImage> fiducialImages;
	
	FiducialTracker	fiducials;	
//	ofxFiducialBacked::gui::GuiGraph fiducial_gui;
#endif

#ifdef USE_SKIN_DETECTOR
	SkinDetector	skin;	
#endif
};
