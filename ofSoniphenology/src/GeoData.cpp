#include "GeoData.h"

//--------------------------------------------------------------
GeoData::GeoData()
{
	dataSourceName	= "PG:dbname=phenology host=opihi.cs.uvic.ca user=postgis password=p0stg1s";
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
	OGRRegisterAll();
	
    datasource = OGRSFDriverRegistrar::Open(dataSourceName.c_str(), FALSE);
    if(datasource!=NULL)
		layer = datasource->GetLayerByName(layerName.c_str());
}

//--------------------------------------------------------------
void
GeoData::destroy()
{
	if (datasource != NULL)
		OGRDataSource::DestroyDataSource(datasource);
}

//--------------------------------------------------------------
void
GeoData::query(ofPoint tlCorner, ofPoint brCorner, ofPoint timeInterval)
{
	if (datasource==NULL || layer==NULL)
		return;

    OGRFeature *feature;

    layer->ResetReading();

	layer->SetSpatialFilterRect(tlCorner.x, tlCorner.y,
								brCorner.x, brCorner.y);

	layer->SetAttributeFilter("first_bloom > 'January 1, 2004'");

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
}
