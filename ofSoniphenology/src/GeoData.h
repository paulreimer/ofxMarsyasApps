#pragma once

#include "settings.h"

#ifdef USE_GEO_DATA
#include "ogrsf_frmts.h"
#endif

#include "ofTypes.h"
#include "ofxThread.h"
#include "ofAppRunner.h"

class GeoData
: public ofxThread
{
	friend class SonificationEngine;
public:
	GeoData();
	~GeoData();
	
	void setup();
	void destroy();

	void query(int tag,
			   ofPoint nwCorner,
			   ofPoint seCorner,
			   ofPoint timeInterval);

	string dataSourceName;
	string layerName;

protected:
	void threadedFunction();

	struct request_t
	{
		float	latitudeMin,	latitudeMax;
		float	longitudeMin,	longitudeMax;
		int		yearMin,		yearMax;
	};
	
	struct response_t
	{
//		vector<ofPoint> points;
		vector<string> dates;
	};
	
private:
#ifdef USE_GEO_DATA
	OGRDataSource	*datasource;
    OGRLayer		*layer;
#endif
	map<int, request_t> requests;
	map<int, response_t> responses;
	
public:	
	map<int, response_t>& getResponses() { return responses; }

};
