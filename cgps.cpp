#include "cgps.h"

CGps::CGps(QString filename,bool useLocalTZ,int tzData)
{
	loaded=false;
	localtz=useLocalTZ;
	tzdata=tzData;
    fn=filename;
 }

int CGps::parsefile()
{
    return -1;
}

int CGps::getTZ(QString lat, QString lon,QDateTime ts)
{
	//Example http://api.geonames.org/timezone?lat=47.01&lng=10.2&username=demo
	//or http://api.geonames.org/timezoneJSON?lat=47.01&lng=10.2&username=demo 
//	QString querystring=
	return 0;
}
void CGps::convertTZ(QDateTime *ts)
{
	if(localtz){
		ts->setTimeSpec(Qt::LocalTime); // mark the timestamp as UTC (but don't convert it)
		*ts = ts->toUTC(); // convert to local time
	}else{
		*ts=ts->addSecs(-3600*tzdata);
	}
}

void CGps::addElement(double lat,double lon,double elevation,QDateTime time)
{
	//ToDo: add the elevation to the gpsLocation class
	gpsLocation loc(lat,lon);
	addElement(loc,time);
}

void CGps::addElement(gpsLocation loc,QDateTime time)
{
	//ToDo: implement the track list
	//		and the addElement function
	// !! check for duplicate timestamps
}

int CGps::searchElement(QString timestamp)
{
	return searchElement(QDateTime::fromString(timestamp,"yyyy-MM-dd hh:mm:ss.zzz"));
}

int CGps::searchElement(QDateTime timestamp)
{
	//ToDo: implement the searchElement function
	// it should search the track list for the current timestamp
	// If the position is between two waypoints, it can be interpolated...
	//
	// The result should be in the gpsPosition position variable
	// Return OK, NotFound or Outside

	convertTZ(&timestamp);
	//...search the position...
	position.setLat(0);
	position.setLng(0);
	return OK;
}