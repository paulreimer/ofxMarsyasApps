#include "MarMicArray.h"

#define UNSET -1

//--------------------------------------------------------------
MarMicArray::MarMicArray()
{
	disableAllEvents();
	ofAddListener(ofEvents.setup, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_setup);
	ofAddListener(ofEvents.draw, (ofxMSAInteractiveObject*)this, &ofxMSAInteractiveObject::_draw);	
}

//--------------------------------------------------------------
MarMicArray::~MarMicArray()
{
	destroy();
}

//--------------------------------------------------------------
void
MarMicArray::setup()
{
	// -------------------------------------------------------------------------
	// Initialize variables
	nTicks = 0;
	angle = 0;

	window_size = 512;
	fs = 48000;
	c = 343.371; 
	iterations = 100;
	
	// Position of microphones (relative linear array)
	pos0 = 0.00;
	pos1 = 0.02;
	pos2 = 0.04;
	pos3 = 0.06;

	// Time thresholds based on distance between each microphone
	tdoa_thres01 = abs(pos0-pos1)*fs/c;
	tdoa_thres02 = abs(pos0-pos2)*fs/c;
	tdoa_thres03 = abs(pos0-pos3)*fs/c;
	tdoa_thres12 = abs(pos1-pos2)*fs/c;
	tdoa_thres13 = abs(pos1-pos3)*fs/c;
	tdoa_thres23 = abs(pos2-pos3)*fs/c;
	
	thres01[0] = window_size/2 - tdoa_thres01;
	thres01[1] = window_size/2 + tdoa_thres01;

	thres02[0] = window_size/2 - tdoa_thres02;
	thres02[1] = window_size/2 + tdoa_thres02;

	thres03[0] = window_size/2 - tdoa_thres03;
	thres03[1] = window_size/2 + tdoa_thres03;

	thres12[0] = window_size/2 - tdoa_thres12; 
	thres12[1] = window_size/2 + tdoa_thres12;
	
	thres13[0] = window_size/2 - tdoa_thres13;
	thres13[1] = window_size/2 + tdoa_thres13;		

	thres23[0] = window_size/2 - tdoa_thres23;
	thres23[1] = window_size/2 + tdoa_thres23;

/* 
	// Time thresholds based on distance between each microphone
	tdoa_thres01 = abs(pos0-pos1)*fs/c;
	tdoa_thres02 = abs(pos0-pos2)*fs/c;
	tdoa_thres03 = abs(pos0-pos3)*fs/c;
	tdoa_thres12 = abs(pos1-pos2)*fs/c;
	tdoa_thres13 = abs(pos1-pos3)*fs/c;
	tdoa_thres23 = abs(pos2-pos3)*fs/c;
	
	thres01[0] = window_size/2 - ceil(tdoa_thres01-FLT_EPSILON);
	thres01[1] = window_size/2 + ceil(tdoa_thres01-FLT_EPSILON);
	
	thres02[0] = window_size/2 - ceil(tdoa_thres02-FLT_EPSILON);
	thres02[1] = window_size/2 + ceil(tdoa_thres02-FLT_EPSILON);
	
	thres03[0] = window_size/2 - ceil(tdoa_thres03-FLT_EPSILON);
	thres03[1] = window_size/2 + ceil(tdoa_thres03-FLT_EPSILON);
	
	thres12[0] = window_size/2 - ceil(tdoa_thres12-FLT_EPSILON);
	thres12[1] = window_size/2 + ceil(tdoa_thres12-FLT_EPSILON);
	
	thres13[0] = window_size/2 - ceil(tdoa_thres13-FLT_EPSILON);
	thres13[1] = window_size/2 + ceil(tdoa_thres13-FLT_EPSILON);
	
	thres23[0] = window_size/2 - ceil(tdoa_thres23-FLT_EPSILON);
	thres23[1] = window_size/2 + ceil(tdoa_thres23-FLT_EPSILON);
*/
	
	// -------------------------------------------------------------------------	
	// Cross Correlation
	MarSystem* seriesCC = mng.create("Series","seriesCC");
	seriesCC->addMarSystem(mng.create("Selector","channelSelect"));
	seriesCC->addMarSystem(mng.create("CrossCorrelation","cc"));
	seriesCC->updctrl("CrossCorrelation/cc/mrs_string/mode","ml"); //ml, phat, general
	seriesCC->addMarSystem(mng.create("Gain","g1"));
	seriesCC->addMarSystem(mng.create("Gain","g2"));
	seriesCC->addMarSystem(mng.create("Gain","g3"));
	mng.registerPrototype("seriesCC",seriesCC);
	
	// -------------------------------------------------------------------------
	// Add Fanout (to 6 channels - one for each mic combination)
	MarSystem* micFanout = mng.create("Fanout","micFanout");
	micFanout->addMarSystem(mng.create("seriesCC","cc01"));
	micFanout->addMarSystem(mng.create("seriesCC","cc02"));
	micFanout->addMarSystem(mng.create("seriesCC","cc03"));
	micFanout->addMarSystem(mng.create("seriesCC","cc12"));
	micFanout->addMarSystem(mng.create("seriesCC","cc13"));
	micFanout->addMarSystem(mng.create("seriesCC","cc23"));
	
	// -------------------------------------------------------------------------	
	// Assemble Network
	updctrl("mrs_natural/inSamples", window_size);
	updctrl("mrs_real/israte", fs);
	
	// -------------------------------------------------------------------------
	// RT Audio Input
	addMarSystem(mng.create("AudioSource","asrc"));
	setctrl("AudioSource/asrc/mrs_natural/nChannels", 4);
	setctrl("AudioSource/asrc/mrs_natural/inSamples", window_size);
	setctrl("AudioSource/asrc/mrs_natural/bufferSize", window_size);
	setctrl("AudioSource/asrc/mrs_real/israte", fs);
	setctrl("AudioSource/asrc/mrs_natural/device", 5);

	addMarSystem(micFanout);
	addMarSystem(mng.create("Gain","g1"));
	
	// -------------------------------------------------------------------------
	// Update Selectors for each Cross Correlation channel
	updctrl("Fanout/micFanout/seriesCC/cc01/Selector/channelSelect/mrs_natural/disable",2);
	updctrl("Fanout/micFanout/seriesCC/cc01/Selector/channelSelect/mrs_natural/disable",3);
	updctrl("Fanout/micFanout/seriesCC/cc02/Selector/channelSelect/mrs_natural/disable",1);
	updctrl("Fanout/micFanout/seriesCC/cc02/Selector/channelSelect/mrs_natural/disable",3);
	updctrl("Fanout/micFanout/seriesCC/cc03/Selector/channelSelect/mrs_natural/disable",1);
	updctrl("Fanout/micFanout/seriesCC/cc03/Selector/channelSelect/mrs_natural/disable",2);
	updctrl("Fanout/micFanout/seriesCC/cc12/Selector/channelSelect/mrs_natural/disable",0);
	updctrl("Fanout/micFanout/seriesCC/cc12/Selector/channelSelect/mrs_natural/disable",3);
	updctrl("Fanout/micFanout/seriesCC/cc13/Selector/channelSelect/mrs_natural/disable",0);
	updctrl("Fanout/micFanout/seriesCC/cc13/Selector/channelSelect/mrs_natural/disable",2);
	updctrl("Fanout/micFanout/seriesCC/cc23/Selector/channelSelect/mrs_natural/disable",0);
	updctrl("Fanout/micFanout/seriesCC/cc23/Selector/channelSelect/mrs_natural/disable",1);
	
	// -------------------------------------------------------------------------	
	// Ready to initialize audio device 
	updctrl("AudioSource/asrc/mrs_bool/initAudio", true);
	
	run();
}

//--------------------------------------------------------------
void
MarMicArray::destroy()
{}

//--------------------------------------------------------------
void
MarMicArray::update()
{
	// Used to scan for maximum value of CC
	double max = 0;
	double max_arg = 0;

	double t01,t02,t03,t12,t13,t23;
	double angle01,angle02,angle03,angle12,angle13,angle23;
	
	double anglesSum = 0.0;
	int anglesConsidered = 0;
	
	int i = nTicks++ % iterations;

	outData01 = getctrl("Fanout/micFanout/seriesCC/cc01/Gain/g2/mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();
	outData02 = getctrl("Fanout/micFanout/seriesCC/cc02/Gain/g2/mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();
	outData03 = getctrl("Fanout/micFanout/seriesCC/cc03/Gain/g2/mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();
	outData12 = getctrl("Fanout/micFanout/seriesCC/cc12/Gain/g2/mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();
	outData13 = getctrl("Fanout/micFanout/seriesCC/cc13/Gain/g2/mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();
	outData23 = getctrl("Fanout/micFanout/seriesCC/cc23/Gain/g2/mrs_realvec/processedData")->to<Marsyas::mrs_realvec>();
	
	// CROSS CORRELATION 0-1
	max = 0;
	max_arg = UNSET;
//	max_arg = window_size/2;
	for (int x = thres01[0]; x < thres01[1]; x++) {
		if (outData01(0,x) > max){
			max = outData01(0,x);
			max_arg = x;
		}
	}
	if (max_arg != UNSET)
	{
		t01 = window_size/2 - max_arg;
		angle01 = (acos(float((t01*c/fs)/(pos1-pos0)))) * (180.0/PI);

		anglesConsidered++;
		anglesSum += angle01;
	}

	// CROSS CORRELATION 0-2
	max = 0;
	max_arg = UNSET;
//	max_arg = window_size/2;
	for (int x = thres02[0]; x < thres02[1]; x++) {
		if (outData02(0,x) > max){
			max = outData02(0,x);
			max_arg = x;
		}
	}
	if (max_arg != UNSET)
	{
		t02 = window_size/2 - max_arg;
		angle02 = acos(float((t02*c/fs)/(pos2-pos0))) * (180.0/PI);
		
		anglesConsidered++;
		anglesSum += angle02;
	}
	
	// CROSS CORRELATION 0-3
	max = 0;
	max_arg = UNSET;
//	max_arg = window_size/2;
	for (int x = thres03[0]; x < thres03[1]; x++) {
		if (outData03(0,x) > max){
			max = outData03(0,x);
			max_arg = x;
		}
	}
	if (max_arg != UNSET)
	{
		t03 = window_size/2 - max_arg;
		angle03 = (acos(float((t03*c/fs)/(pos3-pos0)))) * (180.0/PI);
		
		anglesConsidered++;
		anglesSum += angle03;
	}
	
	// CROSS CORRELATION 1-2
	max = 0;
	max_arg = UNSET;
//	max_arg = window_size/2;
	for (int x = thres12[0]; x < thres12[1]; x++) {
		if (outData12(0,x) > max){
			max = outData12(0,x);
			max_arg = x;
		}
	}
	if (max_arg != UNSET)
	{
		t12 = window_size/2 - max_arg;
		angle12 = (acos(float((t12*c/fs)/(pos2-pos1)))) * (180.0/PI);
		
		anglesConsidered++;
		anglesSum += angle12;
	}
	
	// CROSS CORRELATION 1-3
	max = 0;
	max_arg = UNSET;
//	max_arg = window_size/2;
	for (int x = thres13[0]; x < thres13[1]; x++) {
		if (outData13(0,x) > max){
			max = outData13(0,x);
			max_arg = x;
		}
	}
	if (max_arg != UNSET)
	{
		t13 = window_size/2 - max_arg;
		angle13 = (acos(float((t13*c/fs)/(pos3-pos1)))) * (180.0/PI);
		
		anglesConsidered++;
		anglesSum += angle13;
	}
	
	// CROSS CORRELATION 2-3
	max = 0;
	max_arg = UNSET;
//	max_arg = window_size/2;
	for (int x = thres23[0]; x < thres23[1]; x++) {
		if (outData23(0,x) > max){
			max = outData23(0,x);
			max_arg = x;
		}
	}
	if (max_arg != UNSET)
	{
		t23 = window_size/2 - max_arg;
		angle23 = (acos(float((t23*c/fs)/(pos3-pos2)))) * (180.0/PI);
		
		anglesConsidered++;
		anglesSum += angle23;
	}
	
	if (anglesConsidered)
	{
		angle = ofLerp(angle, anglesSum/anglesConsidered, 0.95);
		cout << "Average angle = " << angle << " degrees" << endl;
	}
}

//--------------------------------------------------------------
void
MarMicArray::draw()
{}
