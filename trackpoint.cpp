#include "trackpoint.h"

#include <QDebug>
#include <QStringList>

/**
  * return true iff this TrackPoint was reached after the TrackPoint t.
  */
bool TrackPoint::later(TrackPoint t) {
    return this->time > t.time;
}

/**
  * return true iff this TrackPoint was reached before the TrackPoint t.
  */
bool TrackPoint::former(TrackPoint t) {
    return this->time < t.time;
}

/**
  * return true iff this TrackPoint was reached at the same time as TrackPoint t.
  */
bool TrackPoint::sameTime(TrackPoint t) {
    return this->time == t.time;
}

/**
  * interpolate position
  */
TrackPoint *TrackPoint::interpolate(TrackPoint const &t1, TrackPoint const &t2, long timePos) {
    double scale = (double)(t2.time - timePos) / (double)(t2.time - t1.time);
    // qDebug() << "scale=" << scale;
    TrackPoint *t = new TrackPoint(timePos,
                t1.lat + (t2.lat - t1.lat) * scale,
                t1.lng + (t2.lng - t1.lng) * scale,
                t1.alt + (t2.alt - t1.alt) * scale);
    return t;
}

bool TrackPoint::operator==(TrackPoint const &t1) const {
    return t1.time == time && t1.lat == lat && t1.lng == lng && t1.alt == alt;
}

QString TrackPoint::toString() {
    return QString("%1: %2/%3/%4").arg(this->time).arg(this->lat).arg(this->lng).arg(this->alt);
}

