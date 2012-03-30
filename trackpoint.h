#pragma once

#include <QString>

class TrackPoint {

public:

    TrackPoint(long time, double lat, double lng, double alt) :
        time(time), lat(lat), lng(lng), alt(alt) {;}

    long   time;
    double lat;
    double lng;
    double alt;

    bool later(TrackPoint t);
    bool former(TrackPoint t);
    bool sameTime(TrackPoint t);

    bool operator==(TrackPoint const &t1) const;

    static  TrackPoint *interpolate(TrackPoint const &t1, TrackPoint const &t2, long timePos);

    QString toString();
};
