#pragma once

/** \mainpage asGPS - A geotagging plugin for AfterShot Pro
 *
 * @author    Andeas Schrell
 * @version   1.3.0
 * @date      2012-06-16
 * @warning   Works only when QtWebKit-Libs are delivered with ASP (since v1.0.1 of ASP)
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
 * @bug    check situations with zero coordinates (windows: black map, linux: water)
 *
 * @todo   quoting of quotes for LIM
 * @todo   timestamp from GPS to time date fields
 * @todo   user documentation
 * @todo   check date and time formats in the edit fields when editing
 * @todo   more comments in the cpp files
 * @todo   check if WSG98 or other standards can be used for input
 * @todo   check possibilities of direction info
 * @todo   configurable start point
 * 
 */

#include <QString>
#include <QWidget>
#include <QLineEdit>
#include <QAbstractButton>
#include <QWebView>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QSpinBox>
#include <QDateTime>

#include "B5Plugin.h"
#include "PluginRevisions.h"
#include "PluginData.h"

#include "WebInfos.h"

#include "iso3166.h"

#include "cgps.h"
#include "gpscsv.h"
#include "gpsgpx.h"

#include "ConfigurationMapper.h"

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
 * @brief     Geotagging in AfterShotPro. The plugin class.
 * @details   This class provides a geotagging plugin for the RAW converter Corel AfterShot Pro.
 * @author    Andeas Schrell
 */

class asGPSplugin : public QObject, public B5Plugin {
        Q_OBJECT

    public:

        /** Constructor for the asGPS plugin.
         * Initializes some values.
         */
        asGPSplugin()
            : m_pHub(NULL), m_pluginId(-1), m_groupId(-1) {
            ;
        }

        /** Gives the priority of our plugin.
         * @returns constant 100.
         */
        int priority() {
            return 100;
        }

        /** Gives the name of our plugin.
         * @returns the plugin name.
         */
        QString name() {
            return QString("asGPS");
        }

        /** Gives the group name of our plugin.
         * @returns the plugin group name.
         */
        QString group() {
            return QString("asGPSplug");
        }

        /** Initialize our asGPS plugin.
         * We have to initialize our class and save some variables from ASP.
         * @returns true, if everything is ok.
         */
        bool init(PluginHub *hub, int id, int groupId, const QString &bundlePath);

        /** Register filters for ASP.
         * @returns true, if fiters could be registered.
         */
        bool registerFilters();

        /** Register options for ASP.
         * @returns true, if options could be registered.
         */
        bool registerOptions();

        /** Setup finished.
         * Connect signals to ASP, check for updates etc.
         * @returns true, if everything was ok.
         */
        bool finish();

        /** Create a plugin dependency.
         * The name must begin with the plugin name ("asGPS::....")
         * @returns a new plugin dependency.
         */
        PluginDependency * createDependency(const QString &);

        /** Gives a list of tool files.
         * @returns a QList of QString
         */
        QList<QString> toolFiles();

        /** Gives a list of widgets.
         * @returns a QList of QWidgets
         */
        QList<QWidget*> toolWidgets();

        /** After tool widget created.
         * After the tool widget was created we are called with this method. Now
         * we can get identifiers from the UI and make adjustments to the UI.
         */
        void toolWidgetCreated(QWidget *uiWidget);

        /** Group ID.
         * A getter for the group id.
         * @returns the integer of the groupId.
         */
        int groupId() const {
            return m_groupId;
        }

        /** Plugin ID.
         * A getter for the plugin id.
         * @returns the integer of the pluginId.
         */
        int pluginId() const {
            return m_pluginId;
        }

        /** Clear tag fields in the asGPS ui.
         * This method clears the IPTC and GPS labels which display the
         * tags from the image under the edit fields. It does not change the tags
         * of the image.
         */
        void clearTags();

    private slots:

        /** Slot which handles the change of the hotness (selected main image) in ASP.
         * When the user changes the hotness image in ASP, this slot is called.
         * @param options are the currently active options/settings of the image.
         */
        void handleHotnessChanged( const PluginImageSettings &options );

        /** Slot which handles the change of settings in ASP.
         * When the user has changed some settings, this slot is called.
         * @param options are the currently active options/settings of the image.
         * @param changed are the just changed options
         * @param layer is the currently active layer
         */
        void handleSettingsChanged(const PluginImageSettings &options, const PluginImageSettings &changed, int layer);

        /** Slot which is called when the map is loaded.
         * This slot is called from the JavaScript part.
         * @param ok true, if the html page war loaded correctly.
         */
        void handleLoadFinished(bool ok);

        /** Slot which is called when enable check box is changed.
         * Called on user click.
         * @param enabled true, if the plugin was enabled by the user.
         */
        void handleCheckedChange(bool enabled);

        /** Slot which is called when external map check box is changed.
         * Called on user click.
         * @param enabled true, if the map should be visible.
         */
        void handleXmapChange(bool enabled);

        /** Slot for the top tristate checkbox in the coords tab.
         * This slot is called, when the user changes the top checkbox in the coordinates tab.
         * It enables the dependent checkboxes in this tab.
         * @param unused The tristate value of the box.
         */
        void handleCoordsCB(int unused);

        /** Slot for the top tristate checkbox in the IPTC tab.
         * This slot is called, when the user changes the top checkbox in the IPTC tab.
         * It enables the dependent checkboxes in this tab.
         * @param unused The tristate value of the box.
         */
        void handleIptcCB(int unused);

        /** Reset all asGPS fields.
         * This slot is called when the user clicks on reset.
         */
        void reset();

        /** Tag the image.
         * When the user clicks on the tag button, this slot is called. It tags the image
         * with the values from the edit fields. It obeys the settings in the tristate
         * check boxes.
         */
        void tagImage();

        /** Geotag with Google Maps.
         * This method uses the current value in the global location edit field and
         * calls the Google Maps API to find the location in the map. It sets the edit
         * fields in the GPS tab with the values from Google.
         */
        void geocode();

        /** Reverse geotag with Google Maps.
         * This method uses the current value in the GPS tab and calls the Google Maps API
         * to find the nearest location. It sets the corresponding values in the IPTC tab.
         */
        void reversegeocode();

        /** Find option IDs.
         * This helper function is used to find the IDs of all used options to speed up
         * the plugin.
         */
        void setOptionIDs();

        /** Connection to JavaScript.
         * This method connects us with the JavaScript part. It adds a JavaScript object \c api, which
         * links to the asGPSplugin class and can be used from JS to call C++ funtions.
         */
        void populateJavaScriptWindowObject();

        /** Fileselector for user country file
         *
         */
        void fileSelectorUserCountries();

        /** Entry for country table was changed.
         */
        void countryTableChanged(QString table);

        /** Update the Google Map.
         * This method updates the Google Map. It locates the map to the correct
         * position according to the GPS location.
         */
        void updateMap();

        /** Set current posotion to start position
          */
        void setAsStartPos();

        /** Home Button
          */
        void home();


    public slots:

        /** Slot for connecting an external browser application.
         * This slot is used to open the given url in an external browser window.
         * We must use this for SSL-connections, because we do not want to
         * distribute the openssl packages with the plugin, only to allow browsing
         * the paypal site.
         * @param url is the url, which should be opened.
         */
        void openExternalBrowser(QUrl url);

        /** Slot for connecting an internal browser window.
         * This slot is used for normal sites like the links in the Google Map
         * which can be opened in a \c QWebView.
         * @param url is the url, which should be opened.
         */
        void openInternalBrowser(QUrl url);

        /** An alert dialog.
         * This method is used to open a message box dialog with correct style. It
         * is mainly used from the JavaScript part in the Maps API. The \c alert() of
         * JavaScript looks very ugly.
         * @param text is the text to display in the info message dialog.
         */
        void alert(QString text);

        /** A marker was clicked.
         * This is the reaction of a click on a marker in the map. The map was centered
         * in the current implementation (JavaScript). In this C++ part it does nothing.
         * @param toolsMap the info which map is used
         */
        void marker_click(bool toolsMap);

        /** A marker was moved (dragged) in the map.
         * This changes the GPS coordinates, so we have to be informed from JavaScript
         * on the new coordinates. We format the latitude and longitude, put them into
         * the edit fields in the GPS tab and set the status to "A", meaning that we have
         * correct GPS values.
         * @param lat latitude in degrees as a double value
         * @param lng longitude in degrees as a double value
         * @param height the height at this location
         * @param toolsMap the info which map is used
         */
        void marker_moved(double lat, double lng, double height, bool toolsMap);

        /** Slot for the JavaScript part to set the country code and country.
         * The Google Maps API provides us with these values.
         * @param short_name is used to set the country code in the IPTC tab.
         * @param long_name is used to set the country name in the IPTC tab.
         */
        void set_country(QString short_name, QString long_name);

        /** Slot for the JavaScript part to set the state.
         * The Google Maps API provides us with these values.
         * @param short_name is unused.
         * @param long_name is used to set the state value in the IPTC tab.
         */
        void set_state(QString short_name, QString long_name);

        /** Slot for the JavaScript part to set the city.
         * The Google Maps API provides us with these values.
         * @param short_name is unused.
         * @param long_name is used to set the city value in the IPTC tab.
         */
        void set_city(QString short_name, QString long_name);

        /** Slot for the JavaScript part to set the location.
         * The Google Maps API provides us with these values.
         * @param long_name is used to set the location in the IPTC tab.
         */
        void set_location(QString long_name);

        /** Web infos are ready to use.
         * The web page we asked for the update infos is loaded, values can be obtained.
         */
        void webInfosReady();

        /** Display the help window.
         * This method displays the window with help info and licenses.
         */
        void displayHelp();

        /** Fill Rawa Data.
         * Fills the Google raw data into the corresponding edit field
         * in the ui.
         */
        void fillGoogleRaw(QString rawData);

        /** Reload all asGPS fields.
         * This slot is called when the user clicks on reload. It reloads
         * all asGPS fields from the GPS/IPTC tags and repositions the map.
         */
        void reload();

        /** Open file dialog to read GPS track.
         */
        void trackFileDialog();

        /** Parse a GPS track.
         */
        void trackLoad();

        /** Update GPS position from the loaded track.
         */
        void trackGetPos(int dummy = 0);

        /**
         * Get number of points in the track.
         */
        int getTrackSize();

        /**
         * Get String with the points lat/lng
         */
        QString getTrackPoint(int n);

        /** Set the optional Layers in the map on/off
          */
        void mapLayers();

    private:

        /** Update the user interface.
         * This method updates all fields in the user interface with the current values.
         * Also the map is updated, if it is enabled.
         * @param options the values which should be set in the ui fields.
         */
        void updateUi(PluginOptionList *options);

        /** Sets the field value with the correct values.
         * We set the infos in the user interface. This variant is used to set the edit fields.
         * @param options the plugin option values.
         * @param field the field to set.
         * @param  optionID the optionID of the option that should be used.
         */
        void setStringField(PluginOptionList *options, QLineEdit *field, int optionID);

        /** Sets the field value with the correct values.
         * We set the infos in the user interface. This variant is used to set the label fields,
         * which display the current GPS/IPTC tags of the image.
         * @param options the plugin option values.
         * @param field the label to set.
         * @param  optionID the optionID of the option that should be used.
         */
        void setStringField(PluginOptionList *options, QLabel *field, int optionID);

        /** Tags the image with one settings value.
         * According to the corresponding tristate box of the edit field the value in the edit field is read and
         * used to set the IPTC/GPS option of the hotness image.
         * @param options the current option list.
         * @param field the field in the user interface which holds the value.
         * @param cb the checkbox in the user interface which gives infos whether the option should be set.
         * @param lab the corresponding label with the IPTC/GPS setting of the image.
         * @param  optionID the optionID of the option that should be used.
         */
        void tag(PluginOptionList *options, QLineEdit *field, QCheckBox *cb, QLabel *lab, int optionID);

        /** Reset the GPS tab to default (empty) values.
         * This method resets the edit fields in the GPS tab of the plugin.
         */
        void resetGPS();

        /** Reset the IPTC tab to default (empty) values.
         * This method resets the edit fields in the IPTC tab of the plugin.
         */
        void resetIPTC();

        /** Reset the Goggle tab to default (empty) values.
         * This method resets the text fields in the Google tab of the plugin.
         */
        void resetGoogle();

        /** WebView not loadable.
         * Display an error that QWebView can not be used.
         */
        void webViewError();

        /** Map initializer.
         * @param view the QWebVie to initialize
         * @param page the QWebPage to use
         * @param toolsMap the info which map is used
         */
        void initMap(QWebView *view, QWebPage *page, bool toolsMap);

        /** IPTC-Daten zusammenstellen.
         */
        QString getIPTCconcat();

        /** Read config file or create it.
         */
        void readAndCreateConfigFile();

        /** Split timestamp in date and time field.
         */
        void splitDate();

        /** Update the map url.
         * This updates the map url on the fly.
         */
        void updateMapUrl(QWebView *view, bool toolsMap);

        /** Check if marker should be gray or red.
         */
        void hideUnhideMarker(QString merk_lat, QString merk_lng, QString merk_alt);

    private:

        PluginHub *m_pHub; /**< out plugin hub, which is our connection to ASP */

        int m_pluginId; /**< this is our current plugin identifier, set by ASP */
        int m_groupId; /**< this is our current plugin group identifier, set by ASP */

        int ID_GPSLatitude; /**< this will hold the id of the EXIF:GPS latitude */
        int ID_GPSLongitude; /**< this will hold the id of the EXIF:GPS longitude */
        int ID_GPSAltitude; /**< this will hold the id of the EXIF:GPS altitude */
        int ID_GPSDateStamp; /**< this will hold the id of the EXIF:GPS date */
        int ID_GPSTimeStamp; /**< this will hold the id of the EXIF:GPS time */
        int ID_GPSStatus; /**< this will hold the id of the EXIF:GPS status */
        int ID_GPSSatellites; /**< this will hold the id of the EXIF:GPS number of satellites */
        int ID_Location; /**< this will hold the id of the IPTC location */
        int ID_CountryCode; /**< this will hold the id of the IPTC country code */
        int ID_Country; /**< this will hold the id of the IPTC country */
        int ID_State; /**< this will hold the id of the IPTC state */
        int ID_City; /**< this will hold the id of the IPTC city */

        QCheckBox *m_enable; /**< this is the enable map check box */
        QLineEdit *m_edit; /**< this is the main edit field for location input */

        QLineEdit *m_lat; /**< the edit field for the latitude */
        QLineEdit *m_lng; /**< the edit field for the longitude */
        QLineEdit *m_alt; /**< the edit field for the altitude */
        QLineEdit *m_date; /**< the edit field for the date */
        QLineEdit *m_time; /**< the edit field for the time */
        QLineEdit *m_status; /**< the edit field for the status */
        QLineEdit *m_sats; /**< the edit field for the number of sats */
        QLineEdit *m_countryCode; /**< the edit field for the country code */
        QLineEdit *m_country; /**< the edit field for the country */
        QLineEdit *m_state; /**< the edit field for the state */
        QLineEdit *m_city; /**< the edit field for the city */
        QLineEdit *m_location; /**< the edit field for the location */

        QAbstractButton *m_reset; /**< button for reset */
        QAbstractButton *m_reload; /**< button for reload */
        QAbstractButton *m_info; /**< button to open the info window */
        QAbstractButton *m_tag; /**< button to tag the image */
        QAbstractButton *m_lim; /**< button for "locate in map" */
        QAbstractButton *m_fnl; /**< button for "find nearest location" */
        QAbstractButton *m_moveMap; /**< button to move map according coordinates */
        QAbstractButton *m_setAsStart; /**< botton to set current pos as start position */
        QAbstractButton *m_home; /**< button for home position */

        QCheckBox *m_coordsCB; /**< global checkbox for GPS coordinates */
        QCheckBox *m_iptcCB; /**< global checkbox for IPTC coordinates */

        QCheckBox *m_latCB; /**< checkbox for the latitude edit field */
        QCheckBox *m_lngCB; /**< checkbox for the longitude edit field */
        QCheckBox *m_altCB; /**< checkbox for the altitude edit field */
        QCheckBox *m_dateCB; /**< checkbox for the date edit field */
        QCheckBox *m_timeCB; /**< checkbox for the time edit field */
        QCheckBox *m_statusCB; /**< checkbox for the status edit field */
        QCheckBox *m_satsCB; /**< checkbox for the satellites edit field */
        QCheckBox *m_countryCodeCB; /**< checkbox for the country code edit field */
        QCheckBox *m_countryCB; /**< checkbox for the country edit field */
        QCheckBox *m_stateCB; /**< checkbox for the state edit field */
        QCheckBox *m_cityCB; /**< checkbox for the city edit field */
        QCheckBox *m_locationCB; /**< checkbox for the location edit field */
        QCheckBox *m_keepMapPos; /**< keep map position for unchecked images */

        QLabel *m_l_lat; /**< info text for current GPS latitude value in image settings */
        QLabel *m_l_lng; /**< info text for current GPS longitude value in image settings */
        QLabel *m_l_alt; /**< info text for current GPS altitude value in image settings */
        QLabel *m_l_date; /**< info text for current GPS date value in image settings */
        QLabel *m_l_time; /**< info text for current GPS time value in image settings */
        QLabel *m_l_status; /**< info text for current GPS status value in image settings */
        QLabel *m_l_sats; /**< info text for current GPS satellites value in image settings */
        QLabel *m_l_countryCode; /**< info text for current IPTC country code value in image settings */
        QLabel *m_l_country; /**< info text for current IPTC country value in image settings */
        QLabel *m_l_state; /**< info text for current IPTC state value in image settings */
        QLabel *m_l_city; /**< info text for current IPTC city value in image settings */
        QLabel *m_l_location; /**< info text for current IPTC location value in image settings */

        //GPS Track
        QAbstractButton *m_t_filebutton;/**< button to browse for a GPS track file */
        QCheckBox *m_t_localTZ; /**< checkbox to use the local time zone */
        QSpinBox *m_t_timezone; /**< spinbox for time zone */
        QSpinBox *m_t_timeoffset; /**< spinbox for time offset */
        QLabel *m_t_status; /**< info text for GPS track read status */
        QLineEdit *m_t_filename; /**< GPS track file name */
        QLineEdit *m_t_lat; /**< GPS track latitude */
        QLineEdit *m_t_lng; /**< GPS track longitude */
        QLineEdit *m_t_alt; /**< GPS track altitude */
        QLabel *m_t_tracktime; /**< display the track time interval */
        QString photoTime; /**< Metadata - photo time */
        //GPS Track

        CGps *track;

        WebInfos *m_webInfos; /**< instance for gettings version infos from the web */

        bool m_autolim; /**< is auto locate in map enabled? */
        bool m_autofnl; /**< is auto find nearest location enabled? */
        bool m_autotag; /**< is auto tagging enabled? */

        QCheckBox *m_cc3; /**< checkbox for 3letter country code */
        QCheckBox *m_openEnabled; /**< checkbox for enabled on startup */
        QCheckBox *m_openExtraMap; /**< checkbox for extra map window on startup */
        QCheckBox *m_allIptc; /**< checkbox for all IPTC fields in search field */
        QCheckBox *m_center; /**< checkbox for automatic map centering on click */
        QCheckBox *m_checkUpdates; /**< checkbox for update check on startup */
        QCheckBox *m_splitGpsTime; /**< should we split date/time in GPS timestamp */

        QLineEdit *m_countryMap; /**< the user defined country table */
        QAbstractButton *m_fileSelect; /**< button for the file selector */

        QLineEdit *m_mapLanguage; /**< edit field to set the map language */
        QLineEdit *m_mapRegion; /**< edit field to set the map region */

        QString m_default_button_style; /**< default button stylesheet as backup */

        Iso3166 m_iso3661; /**< the country code conversion */

        QWebView *m_internalView; /**< map in the toolbar - used to dock */
        QWebView *m_externalView; /**< map in the extra window - used to dock */

        QWebPage *m_internalMapPage; /**< the qweb page with rthe map */
        QWebPage *m_externalMapPage; /**< the qweb page with rthe map */

        QCheckBox *m_xmap; /**< checkbox for external map */

        ConfigurationMapper *m_config; /**< configuration */
        QString m_configDir; /**< configuration directory */

        QTextEdit *m_googleRaw; /**< area to display the Google raw data */
        QTextEdit *m_googleCoordinates; /**< area to display the coordinate data */

        QCheckBox *m_cb_bicycle;
        QCheckBox *m_cb_traffic;
        QCheckBox *m_cb_weather;
        QCheckBox *m_cb_clouds;
        QCheckBox *m_cb_panoramio;

        int m_loaded; /**< counter for loaded web pages */

        bool inSettingsChange;

};
