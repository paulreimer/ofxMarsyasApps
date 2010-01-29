#pragma once

#define	USE_CVD
#define USE_OPENCV
#define	USE_GUI
#define	USE_FIDUCIAL_TRACKER
#undef	USE_SKIN_DETECTOR
#undef	USE_CAMERA

#define	PREFER_OPENCV

#ifdef USE_CVD
#define FAST_CORNER_FUNC	fast_corner_detect_9
#define FAST_CORNER_SCORER	fast_corner_score_9
#endif

#define VIDEO_WIDTH		640
#define VIDEO_HEIGHT	480
#define VIDEO_SIZE		VIDEO_WIDTH,VIDEO_HEIGHT
//#define	VIDEO_FPS		1000/(10)
#define	VIDEO_FPS		120

#ifdef USE_CAMERA
#define CAMERA_WIDTH	640
#define CAMERA_HEIGHT	480
#define CAMERA_SIZE		CAMERA_WIDTH,CAMERA_HEIGHT
#define	CAMERA_FPS		24
#endif
