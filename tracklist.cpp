#include "tracklist.h"

#include <QDebug>
#include <QDateTime>

bool TrackList::binarySearch(const QDateTime time, int &a, int &b) {
    qDebug() << "bs:" << time << ":" << a << "," << b;
    if (b < a) return false;
    if (b - a <= 1) {
        return true;
    }
    if (time <= this->at(a).time) {
        b = a;
        return true;
    }
    if (time >= this->at(b).time) {
        a = b;
        return true;
    }
    int x = (a + b) / 2;
    if (time >= this->at(x).time) {
        a = x;
    } else {
        b = x;
    }
    return binarySearch(time, a, b);
}

TrackPoint TrackList::search(QDateTime time) {
    int a = 0, b = this->size()-1;
    binarySearch(time, a, b);
//    qDebug() << a << b;
    return TrackPoint::interpolate(this->at(a), this->at(b), time);
}

