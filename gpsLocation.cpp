#include "gpsLocation.h"

#include <cmath>
#include <QString>
#include <QStringList>
#include <QDebug>

gpsLocation::gpsLocation(double lat, double lng) {
    this->lat = lat;
    this->lng = lng;
}

gpsLocation::gpsLocation(QString lats, QString lngs) {
    qDebug() << "asGPS:" << lats << "/" << lngs;
    int latSign = 1;
    if (lats.endsWith("S")) {
        latSign = -1;
    }
    lats.resize(lats.size()-1);
    int lngSign = 1;
    if (lngs.endsWith("W")) {
        lngSign = -1;
    }
    lngs.resize(lngs.size()-1);
    QStringList latl = lats.split(",");
    QStringList lngl = lngs.split(",");
    if (latl.size() == 2 && lngl.size() == 2) {
        this->lat = latSign * ( latl.at(0).toDouble() + latl.at(1).toDouble()/60.0 );
        this->lng = lngSign * ( lngl.at(0).toDouble() + lngl.at(1).toDouble()/60.0 );
    } else {
        this->lat = 0;
        this->lng = 0;
    }
    qDebug() << "asGPS: lat =" << this->lat << "/" << "lng =" << this->lng;
}

bool gpsLocation::operator==(const gpsLocation& r) const {
    return r.lat == this->lat && r.lng == this->lng;
}

double gpsLocation::getLat() {
    return this->lat;
}

double gpsLocation::getLng() {
    return this->lng;
}

void gpsLocation::setLat(double lat) {
    this->lat = lat;
}

void gpsLocation::setLng(double lng) {
    this->lng = lng;
}

QStringList gpsLocation::formatAsGoogle(int n) {
    QStringList sl;
    sl << QString("%1").arg(this->lat,0,'f',n);
    sl << QString("%1").arg(this->lng,0,'f',n);
    return sl;
}

QStringList gpsLocation::formatAsOption(int n) {
    QStringList sl;
    int d;
    double m;
    d = std::abs(this->lat);
    m = (std::abs(this->lat) - d)*60;
    sl << QString("%1,%2%3").arg(d).arg(m,n+3,'f',n,'0').arg(this->lat<0?'S':'N');
    d = std::abs(this->lng);
    m = (std::abs(this->lng) - d)*60;
    sl << QString("%1,%2%3").arg(d).arg(m,n+3,'f',n,'0').arg(this->lng<0?'W':'E');
    return sl;
}

QStringList gpsLocation::formatAsDegreesMinutesSeconds(int n) {
    QStringList sl;
    int d, m;
    double r;
    d = std::abs(this->lat);
    r = std::abs(this->lat) - d;
    r *= 60;
    m = r;
    r -= m;
    r *= 60;
    sl << QString("%1° %2' %3%4 %5").arg(d).arg(m,2,'d',0,'0').arg(r,n+3,'f',n,'0').arg('"').arg(this->lat<0?'S':'N');
    d = std::abs(this->lng);
    r = std::abs(this->lng) - d;
    r *= 60;
    m = r;
    r -= m;
    r *= 60;
    sl << QString("%1° %2' %3%4 %5").arg(d).arg(m,2,'d',0,'0').arg(r,n+3,'f',n,'0').arg('"').arg(this->lng<0?'W':'E');
    return sl;
}

