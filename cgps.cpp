#include "cgps.h"

#include <QDebug>

#include "trackpoint.h"
#include "tracklist.h"


CGps::CGps(QString filename, bool useLocalTZ, int tzData) {
	loaded = false;
	localtz = useLocalTZ;
	tzdata = tzData;
	fn = filename;
}

int CGps::parsefile() {
	return -1;
}

int CGps::getTZ(QString lat, QString lon, QDateTime ts) {
	//Example http://api.geonames.org/timezone?lat=47.01&lng=10.2&username=demo
	//or http://api.geonames.org/timezoneJSON?lat=47.01&lng=10.2&username=demo 
//	QString querystring=
	return 0;
}

void CGps::convertTZ(QDateTime *ts) {
	if (localtz) {
		ts->setTimeSpec(Qt::LocalTime); // mark the timestamp as UTC (but don't convert it)
		*ts = ts->toUTC(); // convert to local time
	} else {
		*ts = ts->addSecs(-3600 * tzdata);
	}
}

void CGps::addElement(double lat, double lon, double elevation, QDateTime time, TrackPoint::TP_TYPE type) {
	//ToDo: add the elevation to the gpsLocation class
    qDebug() << "pointTime:" << time;
    TrackPoint loc(time, lat, lon, elevation, type);
    int a=0, b=this->size()-1;
    if (this->binarySearch(time,a,b)) {
        this->insert(b, loc);
    } else {
        this->append(loc);
    }
}

CGps::ParseResult CGps::searchElement(QDateTime timestamp) {
    convertTZ(&timestamp);
    qDebug() << "photoTime:" << timestamp << " positionTime:" << position.time;
    position = this->search(timestamp);
    if (position.time == timestamp)
        return CGps::OK;
    else
        return CGps::Outside;
}
