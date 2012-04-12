#ifndef CONFIGURATIONMAPPER_H
#define CONFIGURATIONMAPPER_H

#include <QString>
#include <QObject>

#include "ConfigFile.h"

class ConfigurationMapper : public QObject {
        Q_OBJECT

    public:

        ConfigurationMapper(QString filename);

        bool startEnabled();
        bool startWithExternalMap();
        bool cc3();
        bool searchAllIPTC();
        bool centerOnClick();
        bool checkForUpdates();
        bool splitGpsTimestamp();
        bool keepMapOnHotnessChange();

        QString countryTable();
        QString mapLanguage();
        QString mapRegion();

        int cbSettingsLat();
        int cbSettingsLng();
        int cbSettingsAlt();
        int cbSettingsDate();
        int cbSettingsTime();
        int cbSettingsStatus();
        int cbSettingsSats();
        int cbSettingsCountryCode();
        int cbSettingsCountry();
        int cbSettingsState();
        int cbSettingsCity();
        int cbSettingsLocation();

        public slots:

        void startEnabled(bool val);
        void startWithExternalMap(bool val);
        void cc3(bool val);
        void searchAllIPTC(bool val);
        void centerOnClick(bool val);
        void checkForUpdates(bool val);
        void splitGpsTimestamp(bool val);
        void keepMapOnHotnessChange(bool val);

        void countryTable(QString cT);
        void mapLanguage(QString language);
        void mapRegion(QString region);

        void cbSettingsLat(int settings);
        void cbSettingsLng(int settings);
        void cbSettingsAlt(int settings);
        void cbSettingsDate(int settings);
        void cbSettingsTime(int settings);
        void cbSettingsStatus(int settings);
        void cbSettingsSats(int settings);
        void cbSettingsCountryCode(int settings);
        void cbSettingsCountry(int settings);
        void cbSettingsState(int settings);
        void cbSettingsCity(int settings);
        void cbSettingsLocation(int settings);

    private:
        ConfigFile *m_cf;

        bool getBool(const char *key, bool def);
        QString getString(const char *key, QString def);
        int getInt(const char *key, int def);

};

#endif // CONFIGURATIONMAPPER_H
