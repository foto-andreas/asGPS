#pragma once

/** \mainpage asGPS - A geotagging plugin for AfterShot Pro
  *
  * @author    Andeas Schrell
  * @version   1.0.0
  * @date      25.01.2012
  * @warning   Works only when QtWebKit-Libs are delivered with ASP
  *
  * \section intro_sec Introduction
  *
  * asGPS is a plug in for the Corel AfterShot Pro RAW converter, which is the 
  * successor of Bibble5 from BibbleLabs. The plugin uses the Google Maps API
  * to get GPS coordinates or infos from GPS coordinates to tag the images with
  * GPS and/or IPTC tags
  *
  * \section install_sec Installation
  *
  * \subsection binary The binary install process
  *
  * The normal user can download asGPS as a binary ASP plugin here:
  * http://schrell.de/asGPS/asGPS.afzplug 
  * From the AfterShot Pro file menu, he can select "install plugin" to install
  * the plugin.
  *
  * A deinstallation can be done by deleting the asGPS.afplugin folder from the
  * plugins folder in the ASP user home.
  *
  * \subsection source The source code 
  *  
  * The source code can be obtained with the git command
  * \verbatim git clone http://schrell.de/asGPS.git \endverbatim
  * or by browsing to the following web page
  * http://schrell.de/cgi-bin/gitweb.cgi?p=asGPS.git
  *
  * \section license License
  * 
  * You may use asGPS free of charge. Small donations for coffee or beer can
  * provided using the Paypal Donate button in the info window of asGPS.
  *
  * asGPS is licensed under the BSD 2-clause license. For the Google Maps API the
  * Google Maps API Terms of Use are also valid for this project as long as it is
  * used.
  *
  * Both licenses are available via the info dialog from the plugin's tool box.
  *
  * \section other Other Information
  *
  * @bug       check situations with zero coordinates
  * @bug       resolve a bug in ASP which need the HELPER setting. 
  *            See \c hotnessChanged().
  */
 
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

/**
  * @brief     asGPS - Geotagging in AfterShotPro.
  * @details   This class provides a geotagging plugin for the RAW converter Corel AfterShot Pro.
  * @author    Andeas Schrell
  */

class asGPSplugin : public QObject, public B5Plugin
{
    Q_OBJECT

public:

    /** Constructor.
      * Initializes some values.
      */
    asGPSplugin() : m_pHub( NULL ), m_pluginId( -1 ), m_groupId( -1 ) { ; }

    /** Gives the priority of our plugin.
      * @returns constant 100.
      */
    virtual int priority() { return 100; }

    /** Gives the name of our plugin.
      * @returns the plugin name.
      */
    virtual QString name() { return QString("asGPS"); }

    /** Gives the group name of our plugin.
      * @returns the plugin group name.
      */
    virtual QString group() { return QString("asGPSplug"); }

    /** Initialize our asGPS plugin.
      * We have to initialize our class and save some variables from ASP.
      * @returns true, if everything is ok.
      */
    virtual bool init(PluginHub *hub, int id, int groupId, const QString &bundlePath);

    /** Register filters for ASP.
      * @returns true, if fiters could be registered.
      */
    virtual bool registerFilters();

    /** Register options for ASP.
      * @returns true, if options could be registered.
      */
    virtual bool registerOptions();

    /** Setup finished.
      * Connect signals to ASP, check for updates etc.
      * @returns true, if everything was ok.
      */
    virtual bool finish();

    /** Create a plugin dependency.
      * The name must begin with the plugin name ("asGPS::....")
      * @returns a new plugin dependency.
      */
    virtual PluginDependency *createDependency(const QString &name);

    /** Gives a list of tool files.
      * @returns a QList of QString
      */
    virtual QList<QString> toolFiles();

    /** Gives a list of widgets.
      * @returns a QList of QWidgets
      */
    virtual QList<QWidget*> toolWidgets();

    /** After tool widget created.
      * After the tool widget was created we are called with this method. Now
      * we can get identifiers from the UI and make adjustments to the UI.
      */
    virtual void toolWidgetCreated(QWidget *uiWidget);

    /** Group ID.
      * A getter for the group id.
      * @returns the integer of the groupId.
      */
    int groupId() const { return m_groupId; }

    /** Plugin ID.
      * A getter for the plugin id.
      * @returns the integer of the pluginId.
      */
    int	pluginId() const { return m_pluginId; }

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
    void openInternalBrowser(QUrl url);
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

    PluginHub   *m_pHub;        /**< out plugin hub, which is our connection to ASP */

    int m_pluginId;             /**< this is our current plugin identifier, set by ASP */
    int m_groupId;              /**< this is our current plugin group identifier, set by ASP */

    int ID_GPSLatitude;         /**< this will hold the id of the EXIF:GPS latitude */
    int ID_GPSLongitude;        /**< this will hold the id of the EXIF:GPS longitude */
    int ID_GPSAltitude;         /**< this will hold the id of the EXIF:GPS altitude */
    int ID_GPSDateStamp;        /**< this will hold the id of the EXIF:GPS date */
    int ID_GPSTimeStamp;        /**< this will hold the id of the EXIF:GPS time */
    int ID_GPSStatus;           /**< this will hold the id of the EXIF:GPS status */
    int ID_GPSSatellites;       /**< this will hold the id of the EXIF:GPS number of satellites */
    int ID_Location;            /**< this will hold the id of the IPTC location */
    int ID_CountryCode;         /**< this will hold the id of the IPTC country code */
    int ID_Country;             /**< this will hold the id of the IPTC country */
    int ID_State;               /**< this will hold the id of the IPTC state */
    int ID_City;                /**< this will hold the id of the IPTC city */

    QWebView    *m_view;        /**< the web view for the Google map */
    QCheckBox   *m_enable;      /**< this is the enable map check box */
    QLineEdit   *m_edit;        /**< this is the main edit field for location input */

    QLineEdit   *m_lat;         /**< the edit field for the latitude */
    QLineEdit   *m_lon;         /**< the edit field for the longitude */
    QLineEdit   *m_alt;         /**< the edit field for the altitude */
    QLineEdit   *m_date;        /**< the edit field for the date */
    QLineEdit   *m_time;        /**< the edit field for the time */
    QLineEdit   *m_status;      /**< the edit field for the status */
    QLineEdit   *m_sats;        /**< the edit field for the number of sats */
    QLineEdit   *m_countryCode; /**< the edit field for the country code */
    QLineEdit   *m_country;     /**< the edit field for the country */
    QLineEdit   *m_state;       /**< the edit field for the state */
    QLineEdit   *m_city;        /**< the edit field for the city */
    QLineEdit   *m_location;    /**< the edit field for the location */

    QAbstractButton *m_reset;   /**< button for reset */
    QAbstractButton *m_info;    /**< button to open the info window */
    QAbstractButton *m_tag;     /**< button to tag the image */
    QAbstractButton *m_lim;     /**< button for "locate in map" */
    QAbstractButton *m_fnl;     /**< button for "find nearest location" */

    QCheckBox *m_coordsCB;      /**< global checkbox for GPS coordinates */
    QCheckBox *m_iptcCB;        /**< global checkbox for IPTC coordinates */

    QCheckBox   *m_latCB;
    QCheckBox   *m_lonCB;
    QCheckBox   *m_altCB;
    QCheckBox   *m_dateCB;
    QCheckBox   *m_timeCB;
    QCheckBox   *m_statusCB;
    QCheckBox   *m_satsCB;
    QCheckBox   *m_countryCodeCB;
    QCheckBox   *m_countryCB;
    QCheckBox   *m_stateCB;
    QCheckBox   *m_cityCB;
    QCheckBox   *m_locationCB;

    QLabel      *m_l_lat;
    QLabel      *m_l_lon;
    QLabel      *m_l_alt;
    QLabel      *m_l_date;
    QLabel      *m_l_time;
    QLabel      *m_l_status;
    QLabel      *m_l_sats;
    QLabel      *m_l_countryCode;
    QLabel      *m_l_country;
    QLabel      *m_l_state;
    QLabel      *m_l_city;
    QLabel      *m_l_location;

    WebInfos    *m_webInfos;

};
