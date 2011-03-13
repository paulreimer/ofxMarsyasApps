#include "testApp.h"

#include "RtAudio.h"
//using namespace RtAudio;

//--------------------------------------------------------------
void
testApp::setup(){
//	ofSoundStreamListDevices();

	RtAudio *audioTemp = 0;
	try {
		audioTemp = new RtAudio();
	} catch (RtError &error) {
		error.printMessage();
	}
 	int devices = audioTemp->getDeviceCount();
	RtAudio::DeviceInfo info;
	for (int i=0; i< devices; i++) {
		try {
			info = audioTemp->getDeviceInfo(i);
		} catch (RtError &error) {
			error.printMessage();
			break;
		}
		if (info.inputChannels==4)
		{
			std::cout << "device = " << i << " (" << info.name << ")\n";
			if (info.isDefaultInput) std::cout << "----* default ----* \n";
			std::cout << "maximum output channels = " << info.outputChannels << "\n";
			std::cout << "maximum input channels = " << info.inputChannels << "\n";
			std::cout << "-----------------------------------------\n";
		}		
	}
	delete audioTemp;

	ofSetBackgroundAuto(true);
	ofBackground(0,0,0);

	ofSetFrameRate(1000/(10));
	glutSetWindowTitle("MarMicArray");
	
	font.loadFont("fonts/HelveticaBold.ttf", 14);
}

//--------------------------------------------------------------
void
testApp::update()
{}

//--------------------------------------------------------------
void testApp::draw()
{
	ofSetColor(0x00ee00);
	ofFill();
	ofCircle(ofMap(net.angle, 0, 180, 0.0, ofGetWidth()), ofGetHeight()/2, 20);
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
testApp::mouseMoved(int x, int y )
{}

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
