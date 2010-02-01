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
public:
	GeoData();
	~GeoData();
	
	void setup();
	void destroy();

	void query(ofPoint tlCorner,
			   ofPoint brCorner,
			   ofPoint timeInterval);

	string dataSourceName;
	string layerName;

protected:
	void threadedFunction();
	
	float	latitudeMin,	latitudeMax;
	float	longitudeMin,	longitudeMax;
	int		yearMin,		yearMax;

private:
#ifdef USE_GEO_DATA
	OGRDataSource	*datasource;
    OGRLayer		*layer;
#endif
	
	bool bNewQuery;
};
