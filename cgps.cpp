#include "cgps.h"

CGps::CGps(QString filename,bool useLocalTZ,int tzData)
{
	localtz=useLocalTZ;
	tzdata=tzData;
    fn=filename;
 }

int CGps::parsetime(QString p_time)
{
    pdt=QDateTime::fromString(p_time,"yyyy-MM-dd hh:mm:ss.zzz");
    return parsefile();
}
int CGps::parsetime(QDateTime pdt_time)
{
    pdt=pdt_time;
    return parsefile();
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
		ts->setTimeSpec(Qt::UTC); // mark the timestamp as UTC (but don't convert it)
		*ts = ts->toLocalTime(); // convert to local time
	}else{
		*ts=ts->addSecs(3600*tzdata);
	}
}