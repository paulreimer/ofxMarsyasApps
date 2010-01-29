#include "testApp.h"

#ifdef USE_CVD
#include <cvd/gl_helpers.h>
#endif

#define VIDEO_WIDTH		640
#define VIDEO_HEIGHT	480
#define VIDEO_SIZE		VIDEO_WIDTH,VIDEO_HEIGHT

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
	font.loadFont(ofToDataPath("HelveticaBold.ttf"), 12);
	font_sm.loadFont(ofToDataPath("Helvetica.ttf"), 9);

	cvGrabber.listDevices();
	cvGrabber.initGrabber(VIDEO_SIZE);
	videoSize.set(VIDEO_SIZE);

	cameraGrabber.initGrabber(640,480);
	cameraImage.allocate(640,480, OF_IMAGE_COLOR);	
	
#ifdef USE_FIDUCIAL_TRACKER
	fiducials.pixels = cvGrabber.getPixels();
	fiducials.videoSize.x = videoSize.x;
	fiducials.videoSize.y = videoSize.y;

	fiducials.width = 200;
	fiducials.height = 200 * VIDEO_HEIGHT/VIDEO_WIDTH;

	fiducials.setup();

	fiducialImages.resize(12);
	for (int i=0; i<fiducialImages.size(); i++)
		fiducialImages[i].loadImage(string("fiducials/")+ofToString(i)+".png");

	mixer.fiducials = &fiducials.tracker.fiducialsList;
#endif
/*
	mixer.addChannel(9, ofToDataPath("flute.wav"));
	mixer.addChannel(2, ofToDataPath("flute2.wav"));
	mixer.addChannel(6, ofToDataPath("pluck.wav"));
*/
	mixer.addChannel(9, ofToDataPath("gravity.wav"));
	mixer.addChannel(2, ofToDataPath("jazz.wav"));
	mixer.addChannel(6, ofToDataPath("prodigy.wav"));

#ifdef USE_SKIN_DETECTOR
	skin.pixels = cvGrabber.getPixels();
	skin.videoSize.x = videoSize.x;
	skin.videoSize.y = videoSize.y;
	
	skin.width = 200;
	skin.height = 200 * VIDEO_HEIGHT/VIDEO_WIDTH;
	
	skin.setup();
#endif
}

//--------------------------------------------------------------
void
testApp::update()
{
	cvGrabber.grabFrame();
	cameraGrabber.grabFrame();
	
	if (cvGrabber.isFrameNew())
	{
		CVD::BasicImage<CVD::Rgb<CVD::byte> > grabRGB((CVD::Rgb<CVD::byte>*)cvGrabber.getPixels(),
													  CVD::ImageRef(videoSize.x, videoSize.y));
		imRGB.copy_from(grabRGB);

#ifdef USE_FIDUCIAL_TRACKER
		fiducials.update();
#endif
#ifdef USE_SKIN_DETECTOR
		skin.update();
#endif
	}
	
	if (cameraGrabber.isFrameNew())
		cameraImage.setFromPixels(cameraGrabber.getPixels(), 640,480, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void
testApp::draw()
{
#ifdef USE_FIDUCIAL_TRACKER
	ofPoint drawVideoSize(fiducials.width,fiducials.height);
	ofPoint scale = drawVideoSize / videoSize;
#endif

	glPushMatrix();
	glPixelZoom(scale.x, -scale.y);

	glRasterPos2f(10 + 0*(drawVideoSize.x+10),
				  10 + 0*(drawVideoSize.y+10));
	glDrawPixels(imRGB);

#ifdef USE_FIDUCIAL_TRACKER
	glRasterPos2f(10 + 1*(drawVideoSize.x+10),
				  10 + 0*(drawVideoSize.y+10));
	glDrawPixels(fiducials.imBW);

	glRasterPos2f(10 + 2*(drawVideoSize.x+10),
				  10 + 0*(drawVideoSize.y+10));
	glDrawPixels(fiducials.imThreshold);
#endif

#ifdef USE_SKIN_DETECTOR
	glRasterPos2f(10 + 0*(drawVideoSize.x+10),
				  10 + 1*(drawVideoSize.y+10));
	glDrawPixels(skin.imBlur);
	
	glRasterPos2f(10 + 1*(drawVideoSize.x+10),
				  10 + 1*(drawVideoSize.y+10));
	glDrawPixels(skin.imSkin);

	glRasterPos2f(10 + 2*(drawVideoSize.x+10),
				  10 + 1*(drawVideoSize.y+10));
	glDrawPixels(skin.imMedian);
	
	glRasterPos2f(10 + 3*(drawVideoSize.x+10),
				  10 + 1*(drawVideoSize.y+10));
	glDrawPixels(skin.imEroded);
#endif
	
	glRasterPos2f(0.,0.);
	glPopMatrix();

#ifdef USE_FIDUCIAL_TRACKER
	if (mixer.fiducials==NULL)
		return;
	
	list<ofxFiducial>& fiducials = *mixer.fiducials;
	list<ofxFiducial>::iterator fiducial;
	
	Marsyas::MarSystem* channel;
	string query;
	
	ofRectangle channelFrame(10,drawVideoSize.y+20, 100,100);
	ofRectangle sliderFrame(0,0, 100,12);
	string fiducialStr, channelNameStr;
	ofPoint textOffset;
	
	double pitch, gain;
/*	
	string pitchStr = ofToString(net.pitch, 2);
	float w = font.stringWidth(pitchStr);
	float h = font.stringHeight(pitchStr);

	font.drawString(pitchStr,
					((float)ofGetWidth() - w) / 2,
					((float)ofGetHeight()/2) + (h/2) + drawVideoSize.y + 20);
*/
	cameraImage.draw(ofGetWidth() - (640/1.5) - 10, ofGetHeight() - (480/1.5) - 10, (640/1.5), (480/1.5));

	for (fiducial = fiducials.begin(); fiducial != fiducials.end(); fiducial++)
	{
		if (fiducial->getId() == 1)
			continue;
		
		// Start searching for SoundFileChannel
		query = "SoundFileChannel/"+ofToString(fiducial->getId());
		channel = mixer.channels->getChildMarSystem(query);
		
		// Try searching for MicrophoneChannel
		if (channel == NULL)
		{
			query = "MicrophoneChannel/"+ofToString(fiducial->getId());
			channel = mixer.channels->getChildMarSystem(query);
			channelNameStr = "Microphone";
		}
		else {
			channelNameStr = channel->getctrl("mrs_string/filename")->to<Marsyas::mrs_string>();
		}
		
		if (channel!=NULL)
		{
			textOffset.set(0,10);
			fiducialStr = "fiducial " + ofToString(fiducial->getId());
			
			gain = channel->getctrl("mrs_real/gain")->to<Marsyas::mrs_real>();
			pitch = channel->getctrl("mrs_real/pitch")->to<Marsyas::mrs_real>();

			fiducialImages[fiducial->getId()].draw(channelFrame.x, channelFrame.y,
												   channelFrame.height, channelFrame.height);
			font_sm.drawString(fiducialStr, channelFrame.x, channelFrame.y + channelFrame.height + 10);

			textOffset.x = channelFrame.x + channelFrame.height + 10;
			textOffset.y += 6;

			sliderFrame.x = textOffset.x;
			sliderFrame.y = channelFrame.y + textOffset.y + font.stringHeight(fiducialStr) - 10;
			
			ofSetColor(0x555555);
			ofRect(sliderFrame.x-1, sliderFrame.y-1, sliderFrame.width+2, sliderFrame.height+2);
			ofFill();
			ofSetColor(0x222222);
			ofRect(sliderFrame.x, sliderFrame.y, sliderFrame.width, sliderFrame.height);
			ofSetColor(0xdddddd);
			ofRect(sliderFrame.x, sliderFrame.y, sliderFrame.width * gain, sliderFrame.height);

			ofSetColor(0xffffff);
			font.drawString(channelNameStr, textOffset.x, channelFrame.y + textOffset.y);
			textOffset.y += font.stringHeight(channelNameStr);
			textOffset.x += sliderFrame.width;
			
			
			font.drawString(", " + ofToString(pitch, 3) + "x", textOffset.x, channelFrame.y + textOffset.y);

			channelFrame.y += channelFrame.height + 20;
		}
	}
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
{}

//--------------------------------------------------------------
void
testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::windowResized(int w, int h)
{}
