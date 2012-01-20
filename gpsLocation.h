#pragma once

#include <QString>
#include <QStringList>

class gpsLocation {

public:

    gpsLocation() : lat(0), lng(0) {}
    gpsLocation(double lat, double lng);
    gpsLocation(QString lats, QString lngs);

    bool operator==(const gpsLocation& r) const;

    double getLat();
    double getLng();

    void setLat(double lat);
    void setLng(double lng);

    QStringList formatAsGoogle(int n); // dd.ddddd
    QStringList formatAsOption(int n); // dd,mm.mmmmmm N
    QStringList formatAsDegreesMinutesSeconds(int n); // dd° mm' ss.sssss'' N

private:

    double lat;
    double lng;

};
