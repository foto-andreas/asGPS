#include "cgps.h"

#include <QDebug>
#include <QTime>

#include "trackpoint.h"
#include "tracklist.h"


CGps::CGps(QString filename) {
	loaded = false;
	fn = filename;
}

int CGps::parsefile() {
	return -1;
}

int CGps::getTZ(QString lat, QString lon, QDateTime ts) {
    Q_UNUSED(lat); Q_UNUSED(lon); Q_UNUSED(ts);
	//Example http://api.geonames.org/timezone?lat=47.01&lng=10.2&username=demo
	//or http://api.geonames.org/timezoneJSON?lat=47.01&lng=10.2&username=demo 
//	QString querystring=
	return 0;
}

void CGps::convertTZ(QDateTime *ts, bool localtz, int tzdata, int offset) {
	if (localtz) {
        ts->setTimeSpec(Qt::LocalTime); // mark the timestamp as LocalTime (but don't convert it)
        *ts = ts->toUTC();
        ts->setTimeSpec(Qt::UTC);
	} else {
        ts->setTimeSpec(Qt::UTC); // mark it as UTC
        *ts = ts->addSecs(-3600 * tzdata);
	}
    *ts = ts->addSecs(offset);
}

void CGps::addElement(double lat, double lon, double elevation, QDateTime time, TrackPoint::TP_TYPE type) {
//    qDebug() << "pointTime:" << time;
    time.setTimeSpec(Qt::UTC);
    TrackPoint loc(time, lat, lon, elevation, type);
    int a=0, b=this->size()-1;
    if (this->binarySearch(time,a,b)) {
        this->insert(b, loc);
    } else {
        this->append(loc);
    }
}

CGps::ParseResult CGps::searchElement(QDateTime timestamp, bool localTZ, int tzData, int offset) {
    convertTZ(&timestamp, localTZ, tzData, offset);
    position = this->search(timestamp);
//    qDebug() << "photoTime:" << timestamp << " positionTime:" << position.time << " TYPE:" << position.type;
    if (position.type != TrackPoint::INVALID) {
        return CGps::OK;
    } else {
        position.time = timestamp; // used for user info
        return CGps::Outside;
    }
}
