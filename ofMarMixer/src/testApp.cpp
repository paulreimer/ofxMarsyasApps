#include "testApp.h"

#ifdef USE_CVD
#include <cvd/gl_helpers.h>
#endif

//--------------------------------------------------------------
void
testApp::setup()
{
//	ofSoundStreamListDevices();
	
	ofSetBackgroundAuto(true);
	ofBackground(0,0,0);

	bg.loadImage("chrome/bg.png");
	bg.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);

	ofSetFrameRate(VIDEO_FPS);

	glutSetWindowTitle("PhaseVocoder");
	
	font.loadFont("fonts/HelveticaBold.ttf", 12);
	font_sm.loadFont("fonts/Helvetica.ttf", 9);

//	cvGrabber.listDevices();
	cvGrabber.setDesiredFrameRate(VIDEO_FPS);
	cvGrabber.initGrabber(VIDEO_SIZE);
	videoSize.set(VIDEO_SIZE);

#ifdef USE_CAMERA
	cameraGrabber.setDesiredFrameRate(24);
	cameraGrabber.initGrabber(CAMERA_SIZE);
	cameraImage.allocate(CAMERA_SIZE, OF_IMAGE_COLOR);	
#endif

#ifdef USE_FIDUCIAL_TRACKER	
	fiducials.pixels = cvGrabber.getPixels();
	fiducials.videoSize.x = videoSize.x;
	fiducials.videoSize.y = videoSize.y;

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
	
#ifdef USE_GUI
#ifdef USE_FIDUCIAL_TRACKER
	gui.addPage("Fiducials");
	gui.addContent("RGB",		imRGB);
	gui.addContent("BW",		fiducials.imBW);
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
#ifdef USE_SKIN_DETECTOR
	gui.addPage("Skin");
	gui.addContent("RGB",		imRGB);
	gui.addContent("Blurred",	skin.imBlur);
	gui.addContent("Median",	skin.imMedian);
	gui.addContent("Dilated",	skin.imDilated);
	gui.addContent("Eroded",	skin.imEroded);
	gui.addContent("Skin Mask",	skin.imSkin);
#endif
#ifdef USE_FAST_CORNERS
	gui.addPage("FAST Corners");
	gui.addContent("RGB",		imRGB);
	gui.addContent("BW",		fastCorners.imBW);
	gui.addContent("Corners",	fastCorners);
#endif
	gui.setup();
#endif
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
	
#ifdef USE_FIDUCIAL_TRACKER
	if (mixer.fiducials==NULL)
		return;
	
	list<ofxFiducial>& fiducials = *mixer.fiducials;
	list<ofxFiducial>::iterator fiducial;
	
	Marsyas::MarSystem* channel;
	string query;
	
	ofRectangle channelFrame(10,200+20, 100,100);
	ofRectangle sliderFrame(0,0, 100,12);
	string fiducialStr, channelNameStr;
	ofPoint textOffset;
	
	double pitch, gain;

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
{}

//--------------------------------------------------------------
void
testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void
testApp::windowResized(int w, int h)
{}
