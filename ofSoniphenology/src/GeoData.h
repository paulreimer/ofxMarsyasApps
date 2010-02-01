#pragma once
#include "ogrsf_frmts.h"

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
	OGRDataSource	*datasource;
    OGRLayer		*layer;
	
	bool bNewQuery;
};
