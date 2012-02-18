#ifndef CONFIGURATIONMAPPER_H
#define CONFIGURATIONMAPPER_H

#include <QString>
#include <QObject>

#include "ConfigFile.h"

class ConfigurationMapper : public QObject
{
    Q_OBJECT

public:

    ConfigurationMapper(QString filename);

    bool startEnabled();
    bool startWithExternalMap();
    bool cc3();
    bool searchAllIPTC();
    bool centerOnClick();
    bool checkForUpdates();

    QString countryTable();
    QString mapLanguage();

public slots:

    void startEnabled(bool val);
    void startWithExternalMap(bool val);
    void cc3(bool val);
    void searchAllIPTC(bool val);
    void centerOnClick(bool val);
    void checkForUpdates(bool val);

    void countryTable(QString cT);
    void mapLanguage(QString language);

private:
    ConfigFile *m_cf;

    bool getBool(const char *key);
    QString getString(const char *key);

};

#endif // CONFIGURATIONMAPPER_H
