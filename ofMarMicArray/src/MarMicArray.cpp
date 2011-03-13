#include "MarMicArray.h"

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
	window_size = 8192*4;
	fs = 16000.0;
	c = 343.371; 

	iterations = 100;
	
	// Position of microphones (relative linear array)
	pos0 = 0.00;
	pos1 = 0.04;
	pos2 = 0.02;
	pos3 = 0.06;

	// Time thresholds based on distance between each microphone
	thres01 = abs(pos0-pos1)*fs/c;
	thres02 = abs(pos0-pos2)*fs/c;
	thres03 = abs(pos0-pos3)*fs/c;
	thres12 = abs(pos1-pos2)*fs/c;
	thres13 = abs(pos1-pos3)*fs/c;
	thres23 = abs(pos2-pos3)*fs/c;

	// ****************************************************************************************************
	//                                            MARSYAS NETWORK
	// ****************************************************************************************************

	updctrl("mrs_natural/inSamples",window_size);        // Window size
	updctrl("mrs_real/israte", fs);                 // Sample rate
		
	// ----------------------------------------------------------------------------------------------------
	// Create Channel Prototype

	MarSystem* channel    = mng.create("Series","channel");
	channel->addMarSystem(mng.create("Gain","g1"));
	channel->addMarSystem(mng.create("Windowing","window"));
	channel->addMarSystem(mng.create("Gain","g2"));
	channel->addMarSystem(mng.create("Gain","g3"));
	mng.registerPrototype("Channel",channel);

	MarSystem* chk = mng.create("Parallel","check");
	chk->addMarSystem(mng.create("Channel","chan0"));
	chk->addMarSystem(mng.create("Channel","chan1"));
	chk->addMarSystem(mng.create("Channel","chan2"));
	chk->addMarSystem(mng.create("Channel","chan3"));

	// ----------------------------------------------------------------------------------------------------
	// Create Cross Correlation (might have to move the control update)

	MarSystem * seriesCC = mng.create("Series","seriesCC");
	seriesCC->addMarSystem(mng.create("Selector","channelSelect"));
	seriesCC->addMarSystem(mng.create("CrossCorrelation","cc"));
	seriesCC->updctrl("CrossCorrelation/cc/mrs_string/mode","ml");
	seriesCC->addMarSystem(mng.create("Gain","g2"));
	seriesCC->addMarSystem(mng.create("Gain","g4"));
	mng.registerPrototype("seriesCC",seriesCC);    


	// ----------------------------------------------------------------------------------------------------
	// Add Fanout (to 6 channels - one for each microphone combination)

	MarSystem* micFanout = mng.create("Fanout","micFanout");
	micFanout->addMarSystem(mng.create("seriesCC","cc01"));
	micFanout->addMarSystem(mng.create("seriesCC","cc02"));
	micFanout->addMarSystem(mng.create("seriesCC","cc03"));
	micFanout->addMarSystem(mng.create("seriesCC","cc12"));
	micFanout->addMarSystem(mng.create("seriesCC","cc13"));
	micFanout->addMarSystem(mng.create("seriesCC","cc23"));    

	// ----------------------------------------------------------------------------------------------------
	// Assemble Network

	MarSystem* asrc = mng.create("AudioSource","asrc");

	asrc->updctrl("mrs_real/israte",		 fs);
	asrc->updctrl("mrs_natural/device",	 4);
	asrc->updctrl("mrs_natural/nChannels", 4);
	asrc->updctrl("mrs_natural/inSamples", window_size);
	asrc->updctrl("mrs_natural/bufferSize",window_size);

	// -------------------------------------------------------------------------
	// Resampling to increase resolution
//	MarSystem* resa = mng.create("ResampleSinc","resa");	
//	resa->setctrl("mrs_real/stretch",	fs	/	16000.);
	//									newfs^		oldfs^
	
	addMarSystem(asrc);
//	addMarSystem(resa);
	addMarSystem(chk);
	addMarSystem(micFanout);
	addMarSystem(mng.create("Gain/g1"));

	// ----------------------------------------------------------------------------------------------------
	// Network Settings

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

	// Initalize Audio Device
	asrc->updctrl("mrs_bool/initAudio", true);

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
	// ****************************************************************************************************
	//                            Get Windows of Marsyas data
	// ****************************************************************************************************

	outData01 = getctrl("Fanout/micFanout/seriesCC/cc01/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();
	outData02 = getctrl("Fanout/micFanout/seriesCC/cc02/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();
	outData03 = getctrl("Fanout/micFanout/seriesCC/cc03/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();
	outData12 = getctrl("Fanout/micFanout/seriesCC/cc12/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();
	outData13 = getctrl("Fanout/micFanout/seriesCC/cc13/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();
	outData23 = getctrl("Fanout/micFanout/seriesCC/cc23/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();


	inData0 = getctrl("Parallel/check/Channel/chan0/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();
	inData1 = getctrl("Parallel/check/Channel/chan1/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();
	inData2 = getctrl("Parallel/check/Channel/chan2/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();
	inData3 = getctrl("Parallel/check/Channel/chan3/Gain/g2/mrs_realvec/processedData")->to<Marsyas::realvec>();

	max = 0;
	for ( int i = (window_size/2 - thres01); i < (window_size/2 + thres01); i++){
		if (outData01(i) > max){
			max = outData01(i);
			maxArg = i;
		}
	}
	t01 = window_size/2 - maxArg;
	cout << "t01 = " << t01 << " samples" << endl;

	max = 0;
	for ( int i = (window_size/2 - thres02); i < (window_size/2 + thres02); i++){
		if (outData02(i) > max){
			max = outData02(i);
			maxArg = i;
		}
	}
	t02 = window_size/2 - maxArg;
	cout << "t02 = " << t02 << " samples" << endl;

	max = 0;
	for ( int i = (window_size/2 - thres03); i < (window_size/2 + thres03); i++){
		if (outData03(i) > max){
			max = outData03(i);
			maxArg = i;
		}
	}
	t03 = window_size/2 - maxArg;
	cout << "t03 = " << t03 << " samples" << endl;

	max = 0;
	for ( int i = (window_size/2 - thres12); i < (window_size/2 + thres12); i++){
		if (outData12(i) > max){
			max = outData12(i);
			maxArg = i;
		}
	}
	t12 = window_size/2 - maxArg;
	cout << "t12 = " << t12 << " samples" << endl;

	max = 0;
	for ( int i = (window_size/2 - thres13); i < (window_size/2 + thres13); i++){
		if (outData13(i) > max){
			max = outData13(i);
			maxArg = i;
		}
	}
	t13 = window_size/2 - maxArg;
	cout << "t13 = " << t13 << " samples" << endl;

	max = 0;
	for ( int i = (window_size/2 - thres23); i < (window_size/2 + thres23); i++){
		if (outData23(i) > max){
			max = outData23(i);
			maxArg = i;
		}
	}
	t23 = window_size/2 - maxArg;
	cout << "t23 = " << t23 << " samples" << endl;

	angle01 = (acos(float((t01*c/fs)/(pos1-pos0)))) * (180.0/pi);
	angle02 = (acos(float((t02*c/fs)/(pos2-pos0)))) * (180.0/pi);
	angle03 = (acos(float((t03*c/fs)/(pos3-pos0)))) * (180.0/pi);
	angle12 = (acos(float((t12*c/fs)/(pos2-pos1)))) * (180.0/pi);
	angle13 = (acos(float((t13*c/fs)/(pos3-pos1)))) * (180.0/pi);
	angle23 = (acos(float((t23*c/fs)/(pos3-pos2)))) * (180.0/pi);

	angle = (angle01 + angle02 + angle03 + angle12 + angle13 + angle23)/6;
//	angle = ofLerp(angle, anglesSum/anglesConsidered, 0.95);

	cout << "angle = " << angle << " degrees" << endl;	
}

//--------------------------------------------------------------
void
MarMicArray::draw()
{}
