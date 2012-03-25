#ifndef GPSCSV_H
#define GPSCSV_H

#include "cgps.h"

#include <QStringList>
#include <QIODevice>
#include <QFile>

class GpsCsv : public CGps
{
public:
    GpsCsv(QString filename,bool useLocalTZ,int tzData);
private:
    int parsefile();
    bool readFile(QIODevice *m_device);
    QStringList parseLine();
    int m_pos;
    QString m_string;
    bool m_eof;
};

#endif // GPSCSV_H
