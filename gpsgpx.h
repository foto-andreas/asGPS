#ifndef GPSGPX_H
#define GPSGPX_H

#include <QString>
#include <QtXml/qdom.h>
#include "cgps.h"

class GpsGpx : public CGps {

    public:
        GpsGpx(QString filename);

    private:
        int parsefile();
        void readElement(QDomElement wpt, double *lat, double *lon, double *elev, QDateTime *timestamp, QString *name);

};

#endif // GPSGPX_H
