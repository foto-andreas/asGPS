#include "trackpoint.h"

#include <QDebug>
#include <QStringList>
#include <QDateTime>

#include <math.h>

TrackPoint::TrackPoint(QString time, QString lats, QString lngs, QString alts) {
    qDebug() << "asGPS:" << lats << "/" << lngs;
    this->time = QDateTime::fromString(time, TimeDate_DEF);
    int latSign = 1;
    if (lats.endsWith("S")) {
        latSign = -1;
    }
    lats.resize(lats.size() - 1);
    QStringList latl = lats.split(",");
    if (latl.size() == 3) {
        this->lat = latSign * (latl.at(0).toDouble() + latl.at(1).toDouble() / 60.0 + latl.at(2).toDouble() / 60.0 / 60.0);
    } else if (latl.size() == 2) {
        this->lat = latSign * (latl.at(0).toDouble() + latl.at(1).toDouble() / 60.0);
    } else {
        this->lat = 0;
    }
    int lngSign = 1;
    if (lngs.endsWith("W")) {
        lngSign = -1;
    }
    lngs.resize(lngs.size() - 1);
    QStringList lngl = lngs.split(",");
    if (lngl.size() == 3) {
        this->lng = lngSign * (lngl.at(0).toDouble() + lngl.at(1).toDouble() / 60.0 + lngl.at(2).toDouble() / 60.0 / 60.0);
    } else if (lngl.size() == 2) {
        this->lng = lngSign * (lngl.at(0).toDouble() + lngl.at(1).toDouble() / 60.0);
    } else {
        this->lng = 0;
    }
    QStringList altl = alts.split("/");
    if (altl.size() == 2) {
        this->alt = altl.at(0).toDouble() / altl.at(1).toDouble();
    } else {
        this->alt = altl.at(0).toDouble();
    }

    qDebug() << "asGPS: lat =" << this->lat << "/" << "lng =" << this->lng;
}

/**
 * return true iff this TrackPoint was reached after the TrackPoint t.
 */
bool TrackPoint::isLater(TrackPoint t) {
    return this->time > t.time;
}

/**
 * return true iff this TrackPoint was reached before the TrackPoint t.
 */
bool TrackPoint::isFormer(TrackPoint t) {
    return this->time < t.time;
}

/**
 * return true iff this TrackPoint was reached at the same time as TrackPoint t.
 */
bool TrackPoint::hasSameTime(TrackPoint t) {
    return this->time == t.time;
}

/**
 * interpolate position
 */
TrackPoint TrackPoint::interpolate(TrackPoint const &t1, TrackPoint const &t2, QDateTime timePos) {
//    qDebug() << "t1.time =" << t1.time << " / t2.time =" << t2.time;
    if (t2.time == t1.time) {
        return t1;
    }
    double scale = 1.0
        - (double) (t2.time.toMSecsSinceEpoch() - timePos.toMSecsSinceEpoch())
            / (double) (t2.time.toMSecsSinceEpoch() - t1.time.toMSecsSinceEpoch());
//    qDebug() << "scale=" << scale;
    TrackPoint t(timePos, t1.lat + (t2.lat - t1.lat) * scale, t1.lng + (t2.lng - t1.lng) * scale,
        t1.alt + (t2.alt - t1.alt) * scale);
    return t;
}

bool TrackPoint::operator==(TrackPoint const &t1) const {
//    qDebug() << "==" << t1.lat << "//" << lat;
//    qDebug() << "==" << t1.lng << "//" << lng;
//    qDebug() << "==" << t1.alt << "//" << alt;
    return (t1.time == time) && fabs(t1.lat - lat) < EPS && fabs(t1.lng - lng) < EPS && fabs(t1.alt - alt) < EPS;
}

QString TrackPoint::toString() const {
    return QString("%1: %2/%3/%4").arg(this->time.toString(TimeDate_DEF)).arg(this->lat).arg(this->lng).arg(this->alt);
}

QStringList TrackPoint::formatAsGoogle(int n) {
    QStringList sl;
    sl << QString("%1").arg(this->lat, 0, 'f', n);
    sl << QString("%1").arg(this->lng, 0, 'f', n);
    sl << QString("%1").arg(this->alt, 0, 'f', n);
    return sl;
}

QStringList TrackPoint::formatAsOption(int n) {
    QStringList sl;
    int d;
    double m;
    d = fabs(this->lat);
    m = (fabs(this->lat) - d) * 60;
    sl << QString("%1,%2%3").arg(d).arg(m, n + 3, 'f', n, '0').arg(this->lat < 0 ? 'S' : 'N');
    d = fabs(this->lng);
    m = (fabs(this->lng) - d) * 60;
    sl << QString("%1,%2%3").arg(d).arg(m, n + 3, 'f', n, '0').arg(this->lng < 0 ? 'W' : 'E');
    sl << QString("%1/100").arg(this->alt * 100, 0, 'f', 0);
    return sl;
}

QStringList TrackPoint::formatAsDegreesMinutesSeconds(int n) {
    QStringList sl;
    int d, m;
    double r;
    d = fabs(this->lat);
    r = fabs(this->lat) - d;
    r *= 60;
    m = r;
    r -= m;
    r *= 60;
    sl
        << QString("%1° %2' %3%4 %5").arg(d).arg(m, 2, 'd', 0, '0').arg(r, n + 3, 'f', n, '0').arg('"').arg(
            this->lat < 0 ? 'S' : 'N');
    d = fabs(this->lng);
    r = fabs(this->lng) - d;
    r *= 60;
    m = r;
    r -= m;
    r *= 60;
    sl
        << QString("%1° %2' %3%4 %5").arg(d).arg(m, 2, 'd', 0, '0').arg(r, n + 3, 'f', n, '0').arg('"').arg(
            this->lng < 0 ? 'W' : 'E');
    sl << QString("%1 m").arg(this->alt, 0, 'f', n);
    return sl;
}
