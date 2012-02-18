#pragma once

#include <QtTest/QTest>

/**
  * @brief     cUnit-Tests
  * @details   This class provides unit tests for asGPS.
  * @author    Andeas Schrell
  */

class Tests : public QObject
{
    Q_OBJECT

    private slots:

        /** Test constructors without parameters. */
        void testNew();

        /** Test constructors with parameters */
        void testLatLng();

        /** Test Google formating */
        void testGoogleFormat();

        /** Test ASP Option formatting */
        void testOptionFormat();

        /** Test degrees minutes seconds format */
        void testDegFormat();

        /** Test country code conversion */
        void testIso3661();

        /** Test config file */
        void testConfig();

        /** Test config mapper */
        void testConfigMapper();

};
