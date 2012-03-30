#pragma once

#include <QMap>
#include <QString>

/** Class to convert between ISO country codes.
  *
  */
class Iso3166
{

public:

    /** Constructor to create an empty instance.
      */
    Iso3166();

    /** Load countries from a resource or local file and add them to the list
      * @param nameOrResource filename or resource name
      */
    bool load(QString nameOrResource = QString(":/text/iso3166.txt"));

    /** Clear the instance.
      */
    void clear();

    /** Convert from 3 to 2 code
      * @param threeLetterCode three letter code
      */
    const QString from3to2(QString threeLetterCode);

    /** Convert from 2 to 3 code
      * @param twoLetterCode two letter code
      */
    const QString from2to3(QString twoLetterCode);

    /** Get the name.
      * @param twoLetterCode two letter code
      */
    const QString name(QString twoLetterCode);

private:

    /** local map from 2 to 3
      */
    QMap<QString,QString> map23;

    /** local map from 3 to 2
      */
    QMap<QString,QString> map32;

    /** local map for the names
      */
    QMap<QString,QString> map2n;

};
