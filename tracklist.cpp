#include "tracklist.h"

#include <QDebug>
#include <QDateTime>

bool TrackList::binarySearch(const QDateTime time, int &a, int &b) {
    if (b < a) return false;
//    qDebug() << "bs:" << time << ":" << this->at(a).time << "," << this->at(b).time;
    if (b - a <= 1) {
        return true;
    }
    if (time < this->at(a).time) {
        return false;
    }
    if (time > this->at(b).time) {
        return false;
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
    int a = 0, b = this->size() - 1;
    if (binarySearch(time, a, b)) {
        return TrackPoint::interpolate(this->at(a), this->at(b), time);
    } else {
        TrackPoint a;
        return a;
    }
}

