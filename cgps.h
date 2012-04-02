#ifndef CGPS_H
#define CGPS_H

#include <QIODevice>
#include <QDateTime>

#include "gpsLocation.h"

class CGps
{
public:
    CGps(QString filename,bool useLocalTZ,int tzData);
	//DEPRECATED!!!!
    //int parsetime(QString p_time);
    //int parsetime(QDateTime pdt_time); 
    virtual int parsefile();
    gpsLocation position;
	bool isLoaded(){return loaded;}
    enum ParseResult { OK, Outside, FileErr, ParseErr, NotFound };
	int searchElement(QString timestamp);
	int searchElement(QDateTime timestamp);
protected:
	bool loaded;
    QString fn;
	bool localtz;
	int tzdata;
    QDateTime pdt;
	int getTZ(QString lat, QString lon, QDateTime ts);
	void convertTZ(QDateTime *ts);
	void addElement(double lat,double lon,double elevation,QDateTime time);
	void addElement(gpsLocation loc,QDateTime time);
};

#endif // CGPS_H
