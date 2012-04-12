#ifndef CGPS_H
#define CGPS_H

#include <QIODevice>
#include <QDateTime>

#include "trackpoint.h"
#include "tracklist.h"

class CGps : public TrackList {
	
public:

    typedef enum  {
        OK, Outside, FileErr, ParseErr, NotFound
    } ParseResult;

    CGps(QString filename);
    virtual int parsefile();
    ParseResult searchElement(QDateTime timestamp, bool localTZ, int tzData, int offset);
    TrackPoint position;
	bool isLoaded() {
		return loaded;
	}

protected:
	bool loaded;
	QString fn;
	QDateTime pdt;
	int getTZ(QString lat, QString lon, QDateTime ts);
    void convertTZ(QDateTime *ts, bool localTZ, int tzData, int offset);
    void addElement(double lat, double lon, double elevation, QDateTime time, TrackPoint::TP_TYPE type, QString name);

};

#endif // CGPS_H
