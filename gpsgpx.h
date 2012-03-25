#ifndef GPSGPX_H
#define GPSGPX_H

#include <QString>
#include "cgps.h"

class GpsGpx : public CGps
{
public:
    GpsGpx(QString filename,bool useLocalTZ,int tzData);
private:
    int parsefile();
};

#endif // GPSGPX_H
