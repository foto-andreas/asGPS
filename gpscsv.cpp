#include "gpscsv.h"

#include <QTextDecoder>
#include <stdlib.h>

GpsCsv::GpsCsv(QString filename, bool useLocalTZ, int tzData) :
    CGps(filename, useLocalTZ, tzData) {
    //m_codec = QTextCodec::codecForLocale();
    m_pos = 0;
    m_string = "";
    m_eof = false;
}
int GpsCsv::parsefile() {
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return FileErr;
    }
    if (!readFile(&file))
        return FileErr;
    file.close();
    QStringList qsl = parseLine();
    int typepos, timepos, latpos, lonpos, elevpos;
    typepos = qsl.indexOf("TYPE");
    timepos = qsl.indexOf("TIME");
    latpos = qsl.indexOf("LAT");
    lonpos = qsl.indexOf("LON");
    elevpos = qsl.indexOf("ALT");
    if (typepos == -1 || timepos == -1 || latpos == -1 || lonpos == -1)
        return ParseErr;
    double lat, lon, elev = 0;
    QDateTime timestamp;
    while (!m_eof) {
        qsl = parseLine();
        if (m_eof)
            break;
        if (qsl.at(typepos) == "P") {
            timestamp = QDateTime::fromString(qsl.at(timepos),
                                              "yyyy-MM-ddThh:mm:ssZ");
            lat = qsl.at(latpos).toDouble();
            lon = qsl.at(lonpos).toDouble();
            if (elevpos > 0)
                elev = qsl.at(elevpos).toDouble();
            addElement(lat, lon, elev, timestamp, TrackPoint::TRACK_POINT);
        }
    }
    loaded = true;
    return OK;
}

bool GpsCsv::readFile(QIODevice *m_device) {

    //READ DATA FROM DEVICE
    if (m_device && m_device->isReadable()) {
        QTextDecoder dec(QTextCodec::codecForLocale());
        m_string = dec.toUnicode(m_device->readAll());
    } else {
        return false;
    }
    return true;
}
QStringList GpsCsv::parseLine() {
    QStringList list;
    list.clear();
    bool quotes = false;
    QString field;
    field.clear();
    QChar c;
    while (m_pos < m_string.length()) {
        c = m_string.at(m_pos);
        if (!quotes && c == '"')
            quotes = true;
        else if (quotes && c == '"') {
            quotes = false;
        } else if (!quotes && c == ',') {
            list << field;
            field.clear();
        } else if ((!quotes) && (c == 0x0A || c == 0x0D)) {
            list << field;
            field.clear();
            m_pos++;
            return list;
        } else {
            field += c;
        }
        m_pos++;
    }
    m_eof = true;
    return list;
}
