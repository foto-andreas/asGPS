#pragma once

#include <QString>
#include <QWidget>
#include <QLineEdit>
#include <QAbstractButton>
#include <QWebView>
#include <QCheckBox>
#include <QLabel>

#include "B5Plugin.h"
#include "PluginRevisions.h"
#include "PluginData.h"

#include "WebInfos.h"

#define ON_GPSLatitude "GPSLatitude"
#define ON_GPSLongitude "GPSLongitude"
#define ON_GPSAltitude "GPSAltitude"
#define ON_GPSDateStamp "GPSDateStamp"
#define ON_GPSTimeStamp "GPSTimeStamp"
#define ON_GPSStatus "GPSStatus"
#define ON_GPSSatellites "GPSSatellites"
#define ON_Location "Location"
#define ON_CountryCode "CountryCode"
#define ON_Country "Country"
#define ON_State "State"
#define ON_City "City"

class asGPSplugin : public QObject, public B5Plugin
{

    Q_OBJECT

public:

    asGPSplugin() : m_pHub( NULL ), m_id( -1 ), m_groupId( -1 ) { ; }
    virtual int priority() { return 100; }
    virtual QString name() { return QString("asGPS"); }
    virtual QString group() { return QString("asGPSplug"); }
    virtual bool init(PluginHub *hub, int id, int groupId, const QString &bundlePath);
    virtual bool registerFilters();
    virtual bool registerOptions();
    virtual bool finish();
    virtual PluginDependency *createDependency(const QString &name);
    virtual QList<QString> toolFiles();
    virtual QList<QWidget*> toolWidgets();
    virtual void toolWidgetCreated(QWidget *uiWidget);

    int groupId() const { return m_groupId; }
    int	pluginId() const { return m_id; }

private slots:

    void handleHotnessChanged( const PluginImageSettings &options );
    void handleSettingsChanged( const PluginImageSettings &options, const PluginImageSettings &changed, int layer );
    virtual void handleLoadFinished ( bool ok);
    virtual void handleLoadStarted ();
    virtual void handlePositionFound(double lat, double lng);
    virtual void handleCheckedChange(bool enabled);

    virtual void handleCoordsCB(int);
    virtual void handleIptcCB(int);

    void resetGPS();
    void resetIPTC();
    void reset();
    void tagImage();
    void geocode();
    void reversegeocode();
    void setOptionIDs();
    void populateJavaScriptWindowObject();

public slots:

    void openExternalBrowser(QUrl url);
    void alert(QString text);
    void marker_click();
    void marker_moved(double lat, double lng);
    void set_country(QString short_name, QString long_name);
    void set_state(QString short_name, QString long_name);
    void set_city(QString short_name, QString long_name);
    void set_location(QString long_name);

    void webInfosReady();

    void displayHelp();

private:

    void updateUi( PluginOptionList *options );
    void updateMap();
    void setStringField( PluginOptionList *options, QLineEdit *field, int optionID);
    void setStringField( PluginOptionList *options, QLabel *field, int optionID);
    void tag( PluginOptionList *options, QLineEdit *field, QCheckBox *cb, QLabel *lab, int optionID);

private:

    PluginHub   *m_pHub;

    int         m_id;
    int         m_groupId;

    int ID_GPSLatitude;
    int ID_GPSLongitude;
    int ID_GPSAltitude;
    int ID_GPSDateStamp;
    int ID_GPSTimeStamp;
    int ID_GPSStatus;
    int ID_GPSSatellites;
    int ID_Location;
    int ID_CountryCode;
    int ID_Country;
    int ID_State;
    int ID_City;

    QCheckBox   *m_enable;
    QLineEdit   *m_edit;
    QLineEdit   *m_lat;
    QLineEdit   *m_lon;
    QLineEdit   *m_alt;
    QLineEdit   *m_date;
    QLineEdit   *m_time;
    QLineEdit   *m_stats;
    QLineEdit   *m_sats;
    QWebView    *m_view;
    QLineEdit   *m_countryCode;
    QLineEdit   *m_country;
    QLineEdit   *m_state;
    QLineEdit   *m_city;
    QLineEdit   *m_location;

    QAbstractButton *m_reset;
    QAbstractButton *m_info;
    QAbstractButton *m_tag;
    QAbstractButton *m_lim; // locate in map
    QAbstractButton *m_fnl; // find nearest location

    QCheckBox *m_coordsCB;
    QCheckBox *m_iptcCB;

    QCheckBox   *m_latCB;
    QCheckBox   *m_lonCB;
    QCheckBox   *m_altCB;
    QCheckBox   *m_dateCB;
    QCheckBox   *m_timeCB;
    QCheckBox   *m_statsCB;
    QCheckBox   *m_satsCB;
    QCheckBox   *m_countryCodeCB;
    QCheckBox   *m_countryCB;
    QCheckBox   *m_stateCB;
    QCheckBox   *m_cityCB;
    QCheckBox   *m_locationCB;

    QLabel   *m_l_lat;
    QLabel   *m_l_lon;
    QLabel   *m_l_alt;
    QLabel   *m_l_date;
    QLabel   *m_l_time;
    QLabel   *m_l_stats;
    QLabel   *m_l_sats;
    QLabel   *m_l_countryCode;
    QLabel   *m_l_country;
    QLabel   *m_l_state;
    QLabel   *m_l_city;
    QLabel   *m_l_location;

    WebInfos* m_webInfos;

    PluginDependency* m_pd;

};
