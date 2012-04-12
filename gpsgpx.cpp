#include "gpsgpx.h"
#include <QFile>
#include <QDebug>
#include "stdlib.h"

GpsGpx::GpsGpx(QString filename)
    : CGps(filename) {
}

int GpsGpx::parsefile() {
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return FileErr;
    }
    QDomDocument doc;
    doc.setContent(&file);
    file.close();
    // GPX  -> WPT
    //		-> RTE  -> RTEPT
    //		-> TRK  -> TRKSEG  -> TRKPT
    QDomElement gpx, l1, l2, l3;
    QString name;
    bool found = false;
    double lat, lon, elevation;
    QDateTime time;
    //GPX
    qDebug() << "parseFile";
    gpx = doc.documentElement();
    if (gpx.tagName() != "gpx") return ParseErr;
    //TRK
    l1 = gpx.firstChildElement();
    while (!l1.isNull()) {
        if (l1.tagName() == "wpt") { //waypoint node
            readElement(l1, &lat, &lon, &elevation, &time, &name);
            addElement(lat, lon, elevation, time, TrackPoint::WAY_POINT, name);
            found = true;
        } else if (l1.tagName() == "rte") { //route node
            l2 = l1.firstChildElement("rtept");
            while (!l2.isNull()) {
                readElement(l2, &lat, &lon, &elevation, &time, &name);
                addElement(lat, lon, elevation, time, TrackPoint::ROUTE_POINT, name);
                found = true;
                l2 = l2.nextSiblingElement("rtept");
            }
        } else if (l1.tagName() == "trk") { //track node
            l2 = l1.firstChildElement("trkseg");
            while (!l2.isNull()) {
                l3 = l2.firstChildElement("trkpt");
                while (!l3.isNull()) {
                    readElement(l3, &lat, &lon, &elevation, &time, &name);
                    addElement(lat, lon, elevation, time, TrackPoint::TRACK_POINT, name);
                    found = true;
                    l3 = l3.nextSiblingElement("trkpt");
                }
                l2 = l2.nextSiblingElement("trkseg");
            }
        }
        l1 = l1.nextSiblingElement();
    }
    if (!found) return ParseErr; //do something if nothing's read...
    loaded = true;
    return OK;
}

void GpsGpx::readElement(QDomElement wpt, double *lat, double *lon, double *elev, QDateTime *timestamp, QString *name) {
    QDomElement data;
    *lat = wpt.attribute("lat").toDouble();
    *lon = wpt.attribute("lon").toDouble();
    data = wpt.firstChildElement("time");
    *timestamp = QDateTime::fromString(data.text(), TimeDate_GPS);
    data = wpt.firstChildElement("ele");
    *elev = data.text().toDouble();
    data = wpt.firstChildElement("name");
    *name = data.text();
}
