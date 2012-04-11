#ifndef CGPS_H
#define CGPS_H

#include <QIODevice>
#include <QDateTime>

#include "gpsLocation.h"
#include "trackpoint.h"
#include "tracklist.h"

class CGps : public TrackList {
	
public:
    typedef enum  {
        OK, Outside, FileErr, ParseErr, NotFound
    } ParseResult;
    CGps(QString filename, bool useLocalTZ, int tzData);
	virtual int parsefile();
    ParseResult searchElement(QDateTime timestamp);
    TrackPoint position;
	bool isLoaded() {
		return loaded;
	}

protected:
	bool loaded;
	QString fn;
	bool localtz;
	int tzdata;
	QDateTime pdt;
	int getTZ(QString lat, QString lon, QDateTime ts);
	void convertTZ(QDateTime *ts);
    void addElement(double lat, double lon, double elevation, QDateTime time, TrackPoint::TP_TYPE type);

};

#endif // CGPS_H
