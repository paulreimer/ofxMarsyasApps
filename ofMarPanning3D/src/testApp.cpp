#include "testApp.h"

//--------------------------------------------------------------
testApp::testApp() :
#ifdef USE_GUI
params_gui("Parameters"),
rotation_gui("Rotation"),
#endif
rotation(manualRotation),
offset(manualOffset),

nearZ(3.),
farZ(30.),
depthZ(-10.)
{}

//--------------------------------------------------------------
void testApp::setup()
{
//	ofSoundStreamListDevices();
	
	ofSetBackgroundAuto(false);
	ofBackground(0,0,0);

	ofSetFrameRate(1000/(10));
	glutSetWindowTitle("MarPanning3D");
	
	font.loadFont("fonts/HelveticaBold.ttf", 14);

	defaultSimpleGuiConfig.fontName = "fonts/Helvetica.ttf";
	defaultSimpleGuiConfig.fontSize = 10;
	defaultSimpleGuiConfig.setup();

	gui.setup();
	
	// brute force init
	manualRotation.set(30, 0, 0);
	offset.set(INITIAL_OFFSET);
	
	defaultSimpleGuiConfig.setup();
	rotation_gui.setup();
	// Sliders to rotate the object on its axes
	rotation_gui.addSlider("X Rotation", rotation.x, ROTATION_RANGE);
	rotation_gui.addSlider("Y Rotation", rotation.y, ROTATION_RANGE);
	rotation_gui.addSlider("Z Rotation", rotation.z, ROTATION_RANGE);
	
	rotation_gui.setPos(8, 8);
	rotation_gui.enableAllEvents();
	
//	gui.setup();
	gui.addSlider("Rotation X", net.rot.x, 0., 360.);
	gui.addSlider("Rotation Y", net.rot.y, 0., 360.);
	gui.addSlider("Rotation Z", net.rot.z, 0., 360.);
	gui.addToggle("Playing", net.play_state);
	
//	gui.setPos(8, rotation_gui.height+16);
//	gui.enableAllEvents();

	net.setup();
}

//--------------------------------------------------------------
void testApp::update()
{}

//--------------------------------------------------------------
void testApp::draw()
{
	glDrawBuffer(GL_BACK);									//draw into both back buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//clear color and depth buffers
	glDrawBuffer(GL_BACK_LEFT);								//draw into back left buffer
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();										//reset projection matrix
	glFrustum(leftCam.leftfrustum, leftCam.rightfrustum,	//set left view frustum
			  leftCam.bottomfrustum, leftCam.topfrustum,
			  nearZ, farZ);
	glTranslatef(leftCam.modeltranslation, 0.0, 0.0);		//translate to cancel parallax
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	{
		glTranslatef(0.0, 0.0, depthZ);						//translate to screenplane
		net.draw();
		ofSetColor(0xffffff);
		glScalef(1./100., -1./100., 1.);
		glTranslatef(0.0, 0.0, 3.);
		gui.draw();
	}
	glPopMatrix();

	glDrawBuffer(GL_BACK_RIGHT);							 //draw into back right buffer
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();										//reset projection matrix
	glFrustum(rightCam.leftfrustum, rightCam.rightfrustum,   //set left view frustum
			  rightCam.bottomfrustum, rightCam.topfrustum,
			  nearZ, farZ);
	glTranslatef(rightCam.modeltranslation, 0.0, 0.0);	   //translate to cancel parallax
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	{
		glTranslatef(0.0, 0.0, depthZ);						//translate to screenplane
		net.draw();
		ofSetColor(0xffffff);
		glScalef(1./100., -1./100., 1.);
		glTranslatef(0.0, 0.0, 3.);
		gui.draw();
	}
	glPopMatrix();

	ofSetupScreen();

	int w = ofGetWidth();
	int h = ofGetHeight();

	ofSetColor(0xeeeeee);
	font.drawString(ofToString(net.rate*512, 3)+string(" Hz"),	w-100,h-20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{
	double fovy					= 45;							//field of view in y-axis
	double aspect				= double(w)/double(h);			//screen aspect ratio
	double screenZ				= 10.0;							//screen projection plane
	double IOD					= 0.5;

    double top					= nearZ*tan(DEG_TO_RAD*fovy/2);		//sets top of frustum based on fovy and near clipping plane
    double right				= aspect*top;					//sets right of frustum based on aspect ratio
    double frustumshift			= (IOD/2)*nearZ/screenZ;
	
    leftCam.topfrustum			= top;
    leftCam.bottomfrustum		= -top;
    leftCam.leftfrustum			= -right + frustumshift;
    leftCam.rightfrustum		= right + frustumshift;
    leftCam.modeltranslation	= IOD/2;
	
    rightCam.topfrustum			= top;
    rightCam.bottomfrustum		= -top;
    rightCam.leftfrustum		= -right - frustumshift;
    rightCam.rightfrustum		= right - frustumshift;
    rightCam.modeltranslation	= -IOD/2;
}
