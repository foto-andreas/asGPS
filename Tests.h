#pragma once

#include <QtTest/QTest>

class Tests : public QObject
{
    Q_OBJECT

    private slots:

        void testNew();
        void testLatLng();
        void testGoogleFormat();
        void testOptionFormat();
        void testDegFormat();
};
