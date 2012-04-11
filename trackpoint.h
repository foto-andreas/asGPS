#pragma once

#include <QString>
#include <QDateTime>

#define EPS 0.0000000000001

class TrackPoint {

public:

    typedef enum {
      TRACK_POINT,
      WAY_POINT,
      ROUTE_POINT,
      INVALID
    } TP_TYPE;

    TrackPoint() :
        type(INVALID) {;}

    TrackPoint(QDateTime time, double lat, double lng, double alt, TP_TYPE type = TRACK_POINT) :
        time(time), lat(lat), lng(lng), alt(alt), type(type) {;}

    TrackPoint(QString time, QString lats, QString lngs, QString alts);

    QDateTime   time;
    double      lat;
    double      lng;
    double      alt;
    TP_TYPE     type;

    bool isLater(TrackPoint t);
    bool isFormer(TrackPoint t);
    bool hasSameTime(TrackPoint t);

    bool operator==(TrackPoint const &t1) const;

    static  TrackPoint interpolate(TrackPoint const &t1, TrackPoint const &t2, QDateTime timePos);

    QString toString() const;

    /** Format Google like.
      * This function formats the GPS position in the form ddd.ddddd
      * @param n sets the number of decimal digits
      */
    QStringList formatAsGoogle(int n);

    /** Format ASP-Option like.
      * This function formats the GPS position in the form ddd,mm.mmmmmm N
      * @param n sets the number of decimal digits
      */
    QStringList formatAsOption(int n);

    /** Format with degrees minutes and seconds.
      * This function formats the GPS position in the form ddd° mm' ss.sssss" N
      * @param n sets the number of decimal digits
      */
    QStringList formatAsDegreesMinutesSeconds(int n);


};
