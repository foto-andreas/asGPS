#pragma once

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

    static void interpolate(TrackPoint t1, TrackPoint t2, long timePos);

    bool operator==(TrackPoint t1) {
        return t1.time == this->time && t1.lat == this->lat && t1.lng == this->lng && t1.alt == this->alt;
    }
};
