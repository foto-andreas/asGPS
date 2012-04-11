#pragma once

#include <QList>
#include <QDateTime>

#include "trackpoint.h"

class TrackList : public QList<TrackPoint>
{

public:
    bool binarySearch(const QDateTime time, int &a, int &b);
    TrackPoint search(QDateTime time);

};
