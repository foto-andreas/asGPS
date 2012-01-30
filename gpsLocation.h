#pragma once

#include <QString>
#include <QStringList>

/**
  * @brief     Geographic calculations and formatting.
  * @details   This class provides formatting and calculations for GPS coordinates.
  * @author    Andeas Schrell
  */

class gpsLocation {

public:

    /** Create am instance of this class.
      * and initialize it to zero values.
      */
    gpsLocation() : lat(0), lng(0) {}

    /** Create an instance of this class.
      * And initialize it with the given coordinates in degrees.
      * @param lat latitude in degrees.
      * @param lng longitude in degrees.
      */
    gpsLocation(double lat, double lng);

    /** Create an instance of this class
      * And initialize it with the given coordinates in ASP option form ddd,mm.mmmmmm E.
      * @param lats QString for latitude formatted like ddd,mm.mmmmm N
      * @param lngs QString for longitude formatted like ddd,mm.mmmmm E
      */
    gpsLocation(QString lats, QString lngs);

    /** Check whether the positions are equal.
      * compare with the given locateion.
      * @param r a gpsLocation to compare with us.
      */
    bool operator==(const gpsLocation& r) const;

    /** Latitude.
      * @returns the latitude value as a double
      */
    double getLat();

    /** Longitude.
      * @returns the longitude value as a double
      */
    double getLng();

    /** Set the latitude.
      * Sets the latitude to the given double value.
      * @param lat the latitude to set.
      */
    void setLat(double lat);

    /** Set the longitude.
      * Sets the longitude to the given double value.
      * @param lng the longitude to set.
      */
    void setLng(double lng);

    /** Format Google like.
      * This function formats the GPS position in the form ddd.ddddd
      * @param n sets the number of decimal digits
      */
    QStringList formatAsGoogle(int n);

    /** Format ASP-Option like.
      * This function formats the GPS position in the form ddd,mm.mmmmmm N
      * @param n sets the number of decimal digits
      */
    QStringList formatAsOption(int n);

    /** Format with degrees minutes and seconds.
      * This function formats the GPS position in the form ddd° mm' ss.sssss" N
      * @param n sets the number of decimal digits
      */
    QStringList formatAsDegreesMinutesSeconds(int n);

private:

    double lat;  /**< latitude */
    double lng;  /**< longitude */

};
