#include "gpsgpx.h"
#include <QFile>
#include <QtXml/qdom.h>
#include <QDebug>
#include "stdlib.h"

GpsGpx::GpsGpx(QString filename,bool useLocalTZ,int tzData):CGps(filename,useLocalTZ,tzData)
{
}

int GpsGpx::parsefile()
{
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            return FileErr;
    }
    QDomDocument doc;
    doc.setContent(&file);
    file.close();
    // GPX -> TRK -> TRKSEG -> TRKPT
    QDomElement gpx,trk,trkseg,trkpt,ptattr;
    int diff,mindiff=10000;
    QString minlat, minlon;
    QDateTime qdt;
    //GPX
    gpx=doc.documentElement();
    if(gpx.tagName()!="gpx")return ParseErr;
    //TRK
    trk=gpx.firstChildElement();
    while(!trk.isNull()&&trk.tagName()!="trk")trk=trk.nextSiblingElement();
    if(trk.isNull())return ParseErr;
    //TRKSEG
    trkseg=trk.firstChildElement();
    while(!trkseg.isNull()){
        if(trkseg.tagName()=="trkseg"){
            //TRKPT
            trkpt=trkseg.firstChildElement();
            while(!trkpt.isNull()){
                ptattr=trkpt.firstChildElement();
                while(!ptattr.isNull()){
                    if(ptattr.tagName()=="time")qdt=QDateTime::fromString(ptattr.text(),"yyyy-MM-ddThh:mm:ssZ");
                    ptattr=ptattr.nextSiblingElement();
                }
                qDebug()<<"Trkpt node"<<trkpt.tagName()<<" : "<<trkpt.attribute("lat")<<"/"<<trkpt.attribute("lon")<<" - "<<qdt.toString("hh:mm:ss")<<endl;
				convertTZ(&qdt);
                diff=abs(qdt.secsTo(pdt));
                if(diff<mindiff){
                    mindiff=diff;
                    minlat=trkpt.attribute("lat");
                    minlon=trkpt.attribute("lon");
                }
                trkpt=trkpt.nextSiblingElement();
            }
        }
        trkseg=trkseg.nextSiblingElement();
    }

    if(mindiff<7500){
		position.setLat(minlat.toDouble());
		position.setLng(minlon.toDouble());
        return OK;
    }
    return NotFound;
}
