#pragma once

#include "settings.h"

#ifdef USE_GEO_DATA
#include "ogrsf_frmts.h"
#endif

#include "ofTypes.h"
#include "ofThread.h"
#include "ofAppRunner.h"

class GeoData
: public ofThread
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

	void unquery(int tag);
	void clear();

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
//		vector<string> dates;
		map<int, int> offsets;
	};
	
private:
#ifdef USE_GEO_DATA
	OGRDataSource	*datasource;
    OGRLayer		*layer;
#endif

	map<int, request_t> requests;
	map<int, response_t> responses;
	
	OGRPolygon MakeRect(double dfMinX, double dfMinY,
						double dfMaxX, double dfMaxY);
};
