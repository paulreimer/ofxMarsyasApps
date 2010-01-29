#pragma once
#include "ogrsf_frmts.h"

#include "ofTypes.h"

class GeoData
{
public:
	GeoData();
	~GeoData();
	
	void setup();
	void destroy();

	void query(ofPoint tlCorner, ofPoint brCorner, ofPoint timeInterval);
	
	string dataSourceName;
	string layerName;

	OGRDataSource *datasource;
    OGRLayer *layer;
};
