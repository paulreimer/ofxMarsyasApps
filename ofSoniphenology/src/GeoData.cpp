#include "GeoData.h"

#define FIRST_BLOOM_COLUMN_IDX	6
#define FULL_BLOOM_COLUMN_IDX	10
//--------------------------------------------------------------
GeoData::GeoData()
{
	dataSourceName	= "PG:dbname=phenology host=opihi.cs.uvic.ca user=postgis password=p0stg1s";
//	dataSourceName	= "PG:dbname=phenology host=localhost user=postgis password=p0stg1s";
//	dataSourceName	= ofToDataPath("lilacs/japan_lilac.shp", true);

	layerName		= "japan_lilac";
}

//--------------------------------------------------------------
GeoData::~GeoData()
{
	destroy();
}

//--------------------------------------------------------------
void
GeoData::setup()
{
#ifdef USE_GEO_DATA
	OGRRegisterAll();
	
    datasource = OGRSFDriverRegistrar::Open(dataSourceName.c_str(), FALSE);
    if(datasource!=NULL)
		layer = datasource->GetLayerByName(layerName.c_str());
#endif

	startThread(true, false); // blocking, non-verbose
}

//--------------------------------------------------------------
void
GeoData::destroy()
{
	if (isThreadRunning())
		stopThread();

#ifdef USE_GEO_DATA
	if (datasource != NULL)
		OGRDataSource::DestroyDataSource(datasource);
#endif
}


//--------------------------------------------------------------
void
GeoData::threadedFunction()
{
#ifdef USE_GEO_DATA
	if (datasource==NULL || layer==NULL)
		return;

	map<int, request_t>::iterator req_iter;
	map<int, response_t>::iterator resp_iter;

	for (;;)
	{
		if (requests.empty())
		{
			ofSleepMillis(20);
			continue;
		}

		req_iter = requests.begin();
		while (req_iter != requests.end())
		{
//			while (!lock())
//			{}
			lock();

			int tag			= req_iter->first;
			request_t req	= req_iter->second;

			++req_iter;
			requests.erase(tag);

			responses[tag].dates.clear();

			unlock();
			
			OGRFeature *feature;

			layer->ResetReading();

			layer->SetSpatialFilterRect(req.latitudeMin, req.longitudeMin,
										req.latitudeMax, req.longitudeMax);

			stringstream where;
			where << "first_bloom >= 'January 1, " << req.yearMin << "'";
			where << " AND ";
			where << "first_bloom <= 'January 1, " << req.yearMax << "'";

			layer->SetAttributeFilter(where.str().c_str());

//--------------------------------------------------------------------------			
			OGRGeometry *geometry;
			OGRPoint *point;
			
			OGRFeatureDefn *fDefn;
			int iField;
			
			fDefn = layer->GetLayerDefn();
			
			for(iField = 0; iField < fDefn->GetFieldCount(); iField++)
			{
				OGRFieldDefn *fieldDefn = fDefn->GetFieldDefn( iField );
				printf("%s [%d]: ", fieldDefn->GetNameRef(), iField);
			}
//--------------------------------------------------------------------------			
			
			string first_bloom;
			while( (feature = layer->GetNextFeature()) != NULL )
			{
				first_bloom = feature->GetFieldAsString(FIRST_BLOOM_COLUMN_IDX);
				lock();
				responses[tag].dates.push_back(first_bloom);
				unlock();
				OGRFeature::DestroyFeature(feature);
			}
		}
	}
#endif
}

//--------------------------------------------------------------
void
GeoData::query(int tag, ofPoint from, ofPoint to, ofPoint timeInterval)
{
//	while (!lock())
//		ofSleepMillis(1);
	
	lock();

	requests[tag].latitudeMin	= from.x;
	requests[tag].latitudeMax	= to.x;

	requests[tag].longitudeMin	= from.y;
	requests[tag].longitudeMax	= to.y;
	
	requests[tag].yearMin		= timeInterval.x;
	requests[tag].yearMax		= timeInterval.y;

	unlock();
}

/*
OGRFeatureDefn *fDefn;
int iField;

fDefn = layer->GetLayerDefn();

for(iField = 0; iField < fDefn->GetFieldCount(); iField++)
{
	OGRFieldDefn *fieldDefn = fDefn->GetFieldDefn(iField);
	
	printf("%s [%d]: ", fieldDefn->GetNameRef(), iField);
	switch (fieldDefn->GetType())
	{
		case OFTInteger:
			printf("%d,", feature->GetFieldAsInteger(iField));
			break;
		case OFTReal:
			printf("%.3f,", feature->GetFieldAsDouble(iField));
			break;
		default:
		case OFTString:
			printf("%s,", feature->GetFieldAsString(iField));
			break;
	}
}

geometry = feature->GetGeometryRef();
if(geometry != NULL 
   && wkbFlatten(geometry->getGeometryType()) == wkbPoint)
{
	point = (OGRPoint*)geometry;
	printf("%.3f,%3.f\n", point->getX(), point->getY());
	
	responses[tag].points.push_back(ofPoint(point->getX(), point->getY()));
}
else
printf( "no point geometry\n" );
*/				
