#include <QtTest/QTest>
#include <QDebug>

#include "Tests.h"

#include "gpsLocation.h"

#include "iso3166.h"


void Tests::testNew() {
    gpsLocation g1, g2(0,0);
    QCOMPARE(g1,g2);
}

void Tests::testLatLng() {
    gpsLocation g(1,2);
    QCOMPARE(g.getLat(),1.0);
    QCOMPARE(g.getLng(),2.0);
    g.setLat(4.0);
    g.setLng(3.0);
    QCOMPARE(g.getLat(),4.0);
    QCOMPARE(g.getLng(),3.0);
}

void Tests::testGoogleFormat() {
    {
        gpsLocation g(12.345, 13.45678);
        QStringList sl = g.formatAsGoogle(5);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12.34500"));
        QCOMPARE(sl.at(1), QString("13.45678"));
    }
    {
        gpsLocation g(-12.345, -13.45678);
        QStringList sl = g.formatAsGoogle(5);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("-12.34500"));
        QCOMPARE(sl.at(1), QString("-13.45678"));
    }
    {
        gpsLocation g(12.34555555, -13.45678888);
        QStringList sl = g.formatAsGoogle(5);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12.34556"));
        QCOMPARE(sl.at(1), QString("-13.45679"));
    }
}

void Tests::testOptionFormat() {
    {
        gpsLocation g(12.5, 13.75);
        QStringList sl = g.formatAsOption(5);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12,30.00000N"));
        QCOMPARE(sl.at(1), QString("13,45.00000E"));
    }
    {
        gpsLocation g(-12.5, 13.75);
        QStringList sl = g.formatAsOption(5);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12,30.00000S"));
        QCOMPARE(sl.at(1), QString("13,45.00000E"));
    }
    {
        gpsLocation g(12.5, -13.75);
        QStringList sl = g.formatAsOption(5);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12,30.00000N"));
        QCOMPARE(sl.at(1), QString("13,45.00000W"));
    }
    {
        gpsLocation g(-12.345, -13.45678);
        QStringList sl = g.formatAsOption(2);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12,20.70S"));
        QCOMPARE(sl.at(1), QString("13,27.41W"));
    }
    {
        gpsLocation g(66.666, -14.1440);
        QStringList sl = g.formatAsOption(2);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("66,39.96N"));
        QCOMPARE(sl.at(1), QString("14,08.64W"));
    }
}

void Tests::testDegFormat() {
    {
        gpsLocation g(12.345, 13.45678);
        QStringList sl = g.formatAsDegreesMinutesSeconds(2);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12° 20' 42.00\" N"));
        QCOMPARE(sl.at(1), QString("13° 27' 24.41\" E"));
    }
    {
        gpsLocation g(-12.345, 13.45678);
        QStringList sl = g.formatAsDegreesMinutesSeconds(2);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12° 20' 42.00\" S"));
        QCOMPARE(sl.at(1), QString("13° 27' 24.41\" E"));
    }
    {
        gpsLocation g(12.345, -13.45678);
        QStringList sl = g.formatAsDegreesMinutesSeconds(2);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("12° 20' 42.00\" N"));
        QCOMPARE(sl.at(1), QString("13° 27' 24.41\" W"));
    }
    {
        gpsLocation g(0.2, 0.016666667);
        QStringList sl = g.formatAsDegreesMinutesSeconds(2);
        qDebug() <<  sl.at(0) << "/" << sl.at(1);
        QCOMPARE(sl.at(0), QString("0° 12' 00.00\" N"));
        QCOMPARE(sl.at(1), QString("0° 01' 00.00\" E"));
    }
}

void Tests::testIso3661() {
    Iso3166 iso;
    iso.load();
    QCOMPARE (iso.from2to3("DE"), QString("DEU"));
    QCOMPARE (iso.from2to3("GB"), QString("GBR"));
    QCOMPARE (iso.from2to3("US"), QString("USA"));
    QCOMPARE (iso.from3to2("DEU"), QString("DE"));
    QCOMPARE (iso.name("DE"), QString("Germany, Federal Republic of"));
}

QTEST_MAIN(Tests)
