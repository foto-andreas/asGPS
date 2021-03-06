#include <QtTest/QTest>
#include <QDebug>
#include <QString>
#include <QStringList>

#include "Tests.h"

#include "gpsLocation.h"
#include "iso3166.h"
#include "ConfigFile.h"
#include "ConfigurationMapper.h"
#include "trackpoint.h"
#include "tracklist.h"

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

void Tests::testConfig() {
    ConfigFile cf("asGPS.conf");
    cf.setValue("TEST1", "VALUE1");
    cf.setValue("TEST2", "VALUE2");
    QCOMPARE (cf.getValue("TEST2"), QString("VALUE2"));
    QCOMPARE (cf.getValue("TEST1"), QString("VALUE1"));
    cf.writeFile();
    ConfigFile cf2("asGPS.conf");
    QCOMPARE (cf2.getValue("TEST2"), QString("VALUE2"));
    QCOMPARE (cf2.getValue("TEST1"), QString("VALUE1"));
    cf2.autoWrite(true);
    cf2.setValue("AUTO1", "V1");
    cf.readFile();
    QCOMPARE (cf.getValue("AUTO1"), QString("V1"));
}

void Tests::testConfigMapper() {
    ConfigurationMapper cm("asGPSmapper.conf");
    cm.cc3(true);
    QCOMPARE (cm.cc3(), true);
}

void Tests::testTrackList() {
    TrackList *tl = new TrackList();
    TrackPoint tp1(QDateTime::fromTime_t(10), 1.0, 2.0, 0.1);
    TrackPoint tp2(QDateTime::fromTime_t(20), 2.0, 3.0, 0.2);
    TrackPoint tp3(QDateTime::fromTime_t(30), 3.0, 5.0, 0.3);
    TrackPoint tp4(QDateTime::fromTime_t(35), 3.5, 2.0, 0.35);
    TrackPoint tp5(QDateTime::fromTime_t(40), 4.0, 2.0, -0.40);
    TrackPoint tp6(QDateTime::fromTime_t(45), 4.5, 2.0, -0.45);
    tl->append(tp1);
    tl->append(tp2);
    tl->append(tp3);
    tl->append(tp4);
    tl->append(tp5);
    tl->append(tp6);
    TrackPoint t = TrackPoint::interpolate(tp1, tp3, QDateTime::fromTime_t(20));
    TrackPoint tc(QDateTime::fromTime_t(20), 2.0, 3.5, 0.2);
    qDebug() << t.toString() << "///" << tc.toString();
    QCOMPARE(t, tc);
    t = tl->search(QDateTime::fromTime_t(15));
    TrackPoint ta(QDateTime::fromTime_t(15), 1.5, 2.5, 0.15);
    qDebug() << t.toString() << "///" << ta.toString();
    QCOMPARE(t, ta);
    t = tl->search(QDateTime::fromTime_t(10));
    qDebug() << t.toString() << "///" << tp1.toString();
    QCOMPARE(t, tp1);
    t = tl->search(QDateTime::fromTime_t(45));
    qDebug() << t.toString() << "///" << tp6.toString();
    QCOMPARE(t, tp6);
    t = tl->search(QDateTime::fromTime_t(99));
    qDebug() << t.toString() << "///" << tp6.toString();
    QCOMPARE(t, tp6);
    t = tl->search(QDateTime::fromTime_t(21));
    TrackPoint tb(QDateTime::fromTime_t(21), 2.1, 3.2, 0.21);
    qDebug() << t.toString() << "///" << tb.toString();
    QCOMPARE(t, tb);

}

QTEST_MAIN(Tests)
