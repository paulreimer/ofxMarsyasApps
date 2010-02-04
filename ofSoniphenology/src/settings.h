#pragma once

//--------------------------------------------------------------
// Features
#undef	USE_CVD
#define USE_OPENCV
#define	USE_GUI
#define	USE_FIDUCIAL_TRACKER
#define	USE_SONIFICATION_ENGINE
#define	USE_GEO_DATA
#undef	USE_CAMERA

//--------------------------------------------------------------
// Video
#define	PREFER_OPENCV

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

//--------------------------------------------------------------
// Fiducials
#ifdef USE_FIDUCIAL_TRACKER
#define FADER_FIDUCIAL_ID	1
#define MAP_FIDUCIAL_ID		0
#endif

#define MIRROR_VERTICAL		true
#define MIRROR_HORIZONTAL	true

// Convenience functions
#define EVEN(n) (bool)((int)n % 2) 
#define ODD(n) (!(bool)((int)n % 2))

#define VALID_FIDUCIAL(f) (f->getX() != -100 || f->getY() != -100)

//--------------------------------------------------------------
// Japan map constants
#define MAP_NW_CORNER_LAT			29.8016
#define MAP_NW_CORNER_LONG			128.22961
#define MAP_SE_CORNER_LAT			46.327984
#define MAP_SE_CORNER_LONG			150.642604

#define MAP_UNITS_LAT				7.65217
#define MAP_UNITS_LONG				8.69565

#define MAP_LAT_RANGE				MAP_SE_CORNER_LAT - MAP_NW_CORNER_LAT
#define MAP_LONG_RANGE				MAP_SE_CORNER_LONG - MAP_NW_CORNER_LONG

//#define QUERY_POINT_OFFSET			4/MAP_UNITS_TO_LONG_DEGREES
#define QUERY_POINT_OFFSET			4

#define MIN_TIMESTAMP_YEAR			2000
#define MAX_TIMESTAMP_YEAR			2010
#define TIMESTAMP_RANGE_YEARS		MAX_TIMESTAMP_YEAR - MIN_TIMESTAMP_YEAR

#define MIN_TIMESTAMP_DAYS			(31+28+31+20)
#define MAX_TIMESTAMP_DAYS			(MIN_TIMESTAMP_DAYS + 70)
#define TIMESTAMP_RANGE_DAYS		MAX_TIMESTAMP_DAYS - MIN_TIMESTAMP_DAYS
