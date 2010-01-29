#include "testApp.h"

//--------------------------------------------------------------
void
testApp::setup()
{
//	ofSoundStreamListDevices();
	
	ofSetBackgroundAuto(true);
	ofBackground(0,0,0);

	ofSetFrameRate(1000/(10));
	glutSetWindowTitle("PhaseVocoder");
	
//	font.loadFont(ofToDataPath("frabk.ttf"), 14);
	font.loadFont(ofToDataPath("HelveticaBold.ttf"), 256);
}

//--------------------------------------------------------------
void
testApp::update()
{}

//--------------------------------------------------------------
void
testApp::draw()
{
	string pitchStr = ofToString(net.pitch, 2);
	float w = font.stringWidth(pitchStr);
	float h = font.stringHeight(pitchStr);

	font.drawString(pitchStr, ((float)ofGetWidth() - w) / 2, ((float)ofGetHeight()/2) + (h/2));
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
{
	net.pitch = ofMap(x, 0, ofGetWidth(), -1.0, 2.0);
}

//--------------------------------------------------------------
void
testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::windowResized(int w, int h)
{}
