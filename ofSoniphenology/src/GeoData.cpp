#include "GeoData.h"

//--------------------------------------------------------------
GeoData::GeoData()
{
//	dataSourceName	= "PG:dbname=phenology host=opihi.cs.uvic.ca user=postgis password=p0stg1s";
//	dataSourceName	= "PG:dbname=phenology host=localhost user=postgis password=p0stg1s";
	dataSourceName	= ofToDataPath("lilacs/japan_lilac.shp", true);

	layerName		= "japan_lilac";
	
	bNewQuery = false;
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

	startThread(false, false); // non-blocking, non-verbose
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

	while (1)
	{
		if (bNewQuery)
		{
			while (!lock())
			{}

			OGRFeature *feature;

			layer->ResetReading();

			layer->SetSpatialFilterRect(latitudeMin, longitudeMin,
										latitudeMax, longitudeMax);

			stringstream where;
			where << "first_bloom >= 'January 1, " << yearMin << "'";
			where << " AND ";
			where << "first_bloom <= 'January 1, " << yearMax << "'";

			layer->SetAttributeFilter(where.str().c_str());

			unlock();

			while( (feature = layer->GetNextFeature()) != NULL )
			{
				OGRFeatureDefn *fDefn;
				int iField;
				OGRGeometry *geometry;
				
				fDefn = layer->GetLayerDefn();
				
				for(iField = 0; iField < fDefn->GetFieldCount(); iField++)
				{
					OGRFieldDefn *fieldDefn = fDefn->GetFieldDefn(iField);
					
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
					OGRPoint *point = (OGRPoint*)geometry;
					printf("%.3f,%3.f\n", point->getX(), point->getY());
				}
				else
					printf( "no point geometry\n" );
				
				OGRFeature::DestroyFeature(feature);				
			}

			bNewQuery = false;
		}
		else ofSleepMillis(20);
	}
#endif
}

//--------------------------------------------------------------
void
GeoData::query(ofPoint from, ofPoint to, ofPoint timeInterval)
{
	while (!lock())
		ofSleepMillis(1);

	latitudeMin		= from.x;
	latitudeMax		= to.x;

	longitudeMin	= from.y;
	longitudeMax	= to.y;
	
	yearMin			= timeInterval.x;
	yearMax			= timeInterval.y;
	
	bNewQuery		= true;

	unlock();
}
