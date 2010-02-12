#include "GeoData.h"

#define FIRST_BLOOM_COLUMN_IDX	6
#define FULL_BLOOM_COLUMN_IDX	10
//--------------------------------------------------------------
GeoData::GeoData()
{
	dataSourceName	= "PG:dbname=phenology host=opihi.cs.uvic.ca user=postgis password=p0stg1s";
//	dataSourceName	= "PG:dbname=phenology host=localhost user=postgis password=p0stg1s";
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
	if (layer != NULL)
		datasource->ReleaseResultSet(layer);

	if (datasource != NULL)
		OGRDataSource::DestroyDataSource(datasource);
#endif
}


//--------------------------------------------------------------
void
GeoData::threadedFunction()
{
#ifdef USE_GEO_DATA
	if (datasource==NULL)
		return;

	map<int, request_t>::iterator req_iter;
	map<int, response_t>::iterator resp_iter;

	double	yyyy,		days;
	int		year_idx,	date_idx;

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

			unlock();

			OGRFeature *feature;

			stringstream q;
			q<<"select"
				<<" extract(year from first_bloom) as yyyy,"
				<<" avg(extract(doy from first_bloom)) as days"
			<<" from"
				<<" japan_lilac"
			<<" where"
				<<" (first_bloom, '0'::interval) overlaps" <<"("
					<< "'" << req.yearMin <<"-01-01'::date,"
					<< "'" << req.yearMax <<"-01-01'::date"
				<<")"
				<<" and"
					<<" ST_Intersects("
						<<"setSRID(Box2D_in('BOX("
							<< req.latitudeMin << " " << req.longitudeMin << ","
							<< req.latitudeMax << " " << req.longitudeMax						
						<<")'),4326),"
						<<"geom"
					<<")"
			<<" group by"
				<<" yyyy"
			<<";";

			layer = datasource->ExecuteSQL(q.str().c_str(), NULL, NULL);

			lock();			
			responses[tag].offsets.clear();
			while( (feature = layer->GetNextFeature()) != NULL )
			{
				yyyy = feature->GetFieldAsDouble(0);
				days = feature->GetFieldAsDouble(1);

				if (yyyy > MIN_TIMESTAMP_YEAR && yyyy < MAX_TIMESTAMP_YEAR &&
					days > MIN_TIMESTAMP_DAYS && days < MAX_TIMESTAMP_DAYS)
				{
					year_idx = yyyy-MIN_TIMESTAMP_YEAR;					
					date_idx = (int)ofMap(days,
										  MIN_TIMESTAMP_DAYS, MAX_TIMESTAMP_DAYS,
										  0, TIMESTAMP_RANGE_DAYS);
					
					responses[tag].offsets[year_idx] = date_idx;
				}

				OGRFeature::DestroyFeature(feature);
			}
			unlock();
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

//--------------------------------------------------------------
void
GeoData::unquery(int tag)
{	
	lock();

	requests.erase(tag);
	responses.erase(tag);
	
	unlock();
}

//--------------------------------------------------------------
void
GeoData::clear()
{	
	lock();
	
//	requests.clear();
	responses.clear();

	unlock();
}

//--------------------------------------------------------------
OGRPolygon
GeoData::MakeRect(double dfMinX, double dfMinY, 
				  double dfMaxX, double dfMaxY)
{
    OGRLinearRing oRing;
    OGRPolygon oPoly;
	
    oRing.addPoint(dfMinX, dfMinY);
    oRing.addPoint(dfMinX, dfMaxY);
    oRing.addPoint(dfMaxX, dfMaxY);
    oRing.addPoint(dfMaxX, dfMinY);
    oRing.addPoint(dfMinX, dfMinY);
	
    oPoly.addRing(&oRing);

	return oPoly;
}
