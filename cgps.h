#ifndef CGPS_H
#define CGPS_H

#include <QIODevice>
#include <QDateTime>

#include "gpsLocation.h"

class CGps
{
public:
    CGps(QString filename,bool useLocalTZ,int tzData);
    int parsetime(QString p_time);
    int parsetime(QDateTime pdt_time);
    gpsLocation position;
    enum ParseResult { OK, FileErr, ParseErr, NotFound };
protected:
    virtual int parsefile();
    QString fn;
	bool localtz;
	int tzdata;
    QDateTime pdt;
	int getTZ(QString lat, QString lon, QDateTime ts);
	void convertTZ(QDateTime *ts);
};

#endif // CGPS_H
