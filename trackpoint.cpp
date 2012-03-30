#include "trackpoint.h"


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
void interpolate(TrackPoint t1, TrackPoint t2, long timePos) {
    double scale = (t2.time - timePos) / (t2.time - t1.time);
    TrackPoint t(timePos,
                t1.lat + (t2.lat - t1.lat) * scale,
                t1.lng + (t2.lng - t1.lng) * scale,
                t1.alt + (t2.alt - t1.alt) * scale);
  //  return t;

}
