#include "asGPSplugin.h"

#include "PluginHub.h"
#include "PluginDependency.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QWebView>
#include <QWebFrame>
#include <QToolButton>
#include <QPushButton>
#include <QWebElement>
#include <QVBoxLayout>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QMessageBox>
#include <QRegExp>
#include <QDesktopServices>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QTextEdit>
#include <QTimer>
#include <QCloseEvent>
#include <QShowEvent>
#include <QGridLayout>

#include "TargetVersion.h"
#include "trackpoint.h"
#include "tracklist.h"
#include "WebInfos.h"
#include "ConfigurationMapper.h"

#include "ToolData.h"

extern "C" BIBBLE_API BaseB5Plugin *b5plugin() { return new asGPSplugin; }

/**
  * @brief     Patched WebPage class
  * @details   This class provides a patched version of QWebPage.
  * @author    Andeas Schrell
  */
class MyWebPage : public QWebPage
{
    /** We are a Chrome. So we can do moves and clicks of markers. */
    virtual QString userAgentForUrl(const QUrl& url) const {
        Q_UNUSED(url);
        return "Chrome/1.0";
    }

};

class MyWebView : public QWebView
{
    public:
        MyWebView(QCheckBox *xmap) {
            this->m_xmap = xmap;
            qDebug() << "asGPS: external map window created.";
        }

    protected:
        virtual void closeEvent(QCloseEvent *event) {
            m_xmap->setChecked(false);
            event->accept();
            qDebug() << "asGPS: external map window closed.";
        }

        virtual QSize sizeHint() const {
            qDebug() << "asGPS: sizeHint";
            return QSize();
        }

    private:
        QCheckBox *m_xmap;

};

class InternalWebView : public QWebView
{
    public:
        InternalWebView(QWidget *parent, asGPSplugin *plugin) : QWebView(parent) {
            this->plugin = plugin;
        }

    protected:
        virtual void showEvent(QShowEvent *event) {
            qDebug() << "asGPS: show Event";
//TEST      plugin->updateMap();
            event->accept();
        }

        virtual QSize sizeHint() const {
            qDebug() << "asGPS: sizeHint";
            return QSize();
        }

    private:
        asGPSplugin *plugin;

};

bool asGPSplugin::init(PluginHub *hub, int id, int groupId, const QString &)
{
    qDebug() << "asGPSplugin::init";

    m_pHub = hub;
    m_pluginId = id;
    m_groupId = groupId;
    m_loaded = 0;
    track = NULL;
    inSettingsChange = false;

    return true;
}

bool asGPSplugin::registerFilters()
{
    qDebug() << "asGPSplugin::registerFilters";
    return true;
}

bool asGPSplugin::registerOptions()
{
    qDebug() << "asGPSplugin::registerOptions";
    return true;
}

bool asGPSplugin::finish()
{
    qDebug() << "asGPSplugin::finish";
    bool b = true;

    b = connect( m_pHub,
                  SIGNAL( hotnessChanged( const PluginImageSettings & ) ),
                  SLOT( handleHotnessChanged( const PluginImageSettings & ) ) );

    b &= connect( m_pHub,
                  SIGNAL( settingsChanged( const PluginImageSettings &, const PluginImageSettings &, int ) ),
                  SLOT( handleSettingsChanged( const PluginImageSettings &, const PluginImageSettings &, int ) ) );

    setOptionIDs();

    // configuration file abstraction with a configuration mapper
    m_configDir = m_pHub->property("pluginStorageHome").toString() + "/asGPS";
    QDir qdir;
    qdir.mkdir(m_configDir);
    QString configPath = m_configDir + "/asGPS.conf";
    m_config = new ConfigurationMapper(configPath);
    if (m_config == NULL) {
        alert(tr("asGPS: configuration file problem with file:") + "<br/>" + configPath);
        return false;
    }

    if (m_config->checkForUpdates()) {
        m_webInfos = new WebInfos("de.schrell.asGPS", "8", TARGET_VERSION_STRING);
        connect(m_webInfos,
                SIGNAL(ready()),
                SLOT(webInfosReady()));

        m_webInfos->fetch();
    }

    m_iso3661.load();

    return b;
}

void asGPSplugin::webInfosReady() {
    qDebug() << "asGPSplugin::webInfosReady:" << m_webInfos->identifier() << m_webInfos->webVersion();
    if (m_webInfos->isWebNewer()) {
        QString text = QString(tr("There is a newer version of %1 available. "
                               "It is version %2. You are running %3. "
                               "You can download it under the following url: <a href='%4'>%4</a>"))
                        .arg(m_webInfos->name(), m_webInfos->webVersion(), TARGET_VERSION_STRING, m_webInfos->link());
        QMessageBox::information(NULL, m_webInfos->name(), text);
    }
    delete m_webInfos;
    m_webInfos = NULL;
}

PluginDependency *asGPSplugin::createDependency(const QString &depName)
{
    qDebug() << "asGPS: createDependency";

    if (depName == "ToolData") {
        ToolData *toolData = new ToolData();
        if (toolData) {
            toolData->owner = name();
            toolData->group = group();
            toolData->ownerId = pluginId();
            toolData->groupId = groupId();
            qDebug() << "asGPS: createDependency ToolData" << toolData;
            return toolData;
        }
    }

    qDebug() << "asGPS: createDependency NULL for" << depName;

    return NULL;
}

QList<QString> asGPSplugin::toolFiles()
{
    qDebug() << "asGPSplugin::toolFiles";
    return QList<QString>();
}

QList<QWidget*> asGPSplugin::toolWidgets()
{
    qDebug() << "asGPSplugin::toolWidgets";
    QList<QWidget*> lstWidgets;
    return lstWidgets;
}

void asGPSplugin::toolWidgetCreated(QWidget *uiWidget)
{
    qDebug() << "asGPSplugin::toolWidgetCreated";

    m_cc3 = uiWidget->findChild<QCheckBox*>("cc3letterCB");
    m_openEnabled = uiWidget->findChild<QCheckBox*>("openEnabledCB");
    m_openExtraMap = uiWidget->findChild<QCheckBox*>("openExtraMapCB");
    m_allIptc = uiWidget->findChild<QCheckBox*>("allIptcCB");
    m_center = uiWidget->findChild<QCheckBox*>("centerCB");
    m_checkUpdates = uiWidget->findChild<QCheckBox*>("checkForUpdates");
    m_splitGpsTime = uiWidget->findChild<QCheckBox*>("splitGpsTimestamp");

    m_countryMap = uiWidget->findChild<QLineEdit*>("countryMap");
    m_fileSelect = uiWidget->findChild<QAbstractButton*>("fileSelectorButton");
    m_mapLanguage = uiWidget->findChild<QLineEdit*>("mapLanguage");
    m_mapRegion = uiWidget->findChild<QLineEdit*>("mapRegion");

    m_enable = uiWidget->findChild<QCheckBox*>("enabled");
    m_xmap = uiWidget->findChild<QCheckBox*>("xMap");

    m_autolim = false;
    m_autofnl = false;
    m_autotag = false;

    m_internalView = new InternalWebView(NULL, this);
    m_externalView = new MyWebView(m_xmap);
    m_externalView->setWindowTitle(tr("AfterShot Pro - asGPS map window"));
    m_externalView->setWindowIcon(m_externalView->icon());
    m_edit = uiWidget->findChild<QLineEdit*>("asGPSText_edit");
    m_reset = uiWidget->findChild<QAbstractButton*>("asGPSReset_button");
    m_reload = uiWidget->findChild<QAbstractButton*>("asGPSReload_button");
    m_tag = uiWidget->findChild<QAbstractButton*>("asGPSTag_button");
    m_default_button_style = m_tag->styleSheet();
    m_info = uiWidget->findChild<QAbstractButton*>("asGPSAbout_button");

    m_lat = uiWidget->findChild<QLineEdit*>("latLineEdit");
    m_lng = uiWidget->findChild<QLineEdit*>("lonLineEdit");
    m_alt = uiWidget->findChild<QLineEdit*>("altLineEdit");
    m_date = uiWidget->findChild<QLineEdit*>("dateLineEdit");
    m_time = uiWidget->findChild<QLineEdit*>("timeLineEdit");
    m_status = uiWidget->findChild<QLineEdit*>("statusLineEdit");
    m_sats = uiWidget->findChild<QLineEdit*>("satellitesLineEdit");
    m_countryCode = uiWidget->findChild<QLineEdit*>("countryCodeLineEdit");
    m_country = uiWidget->findChild<QLineEdit*>("countryLineEdit");
    m_state = uiWidget->findChild<QLineEdit*>("stateLineEdit");
    m_city = uiWidget->findChild<QLineEdit*>("cityLineEdit");
    m_location = uiWidget->findChild<QLineEdit*>("locationLineEdit");
    m_latCB = uiWidget->findChild<QCheckBox*>("latCB");
    m_lngCB = uiWidget->findChild<QCheckBox*>("lonCB");
    m_altCB = uiWidget->findChild<QCheckBox*>("altCB");
    m_dateCB = uiWidget->findChild<QCheckBox*>("dateCB");
    m_timeCB = uiWidget->findChild<QCheckBox*>("timeCB");
    m_statusCB = uiWidget->findChild<QCheckBox*>("statusCB");
    m_satsCB = uiWidget->findChild<QCheckBox*>("satCB");
    m_countryCodeCB = uiWidget->findChild<QCheckBox*>("countryCodeCB");
    m_countryCB = uiWidget->findChild<QCheckBox*>("countryCB");
    m_stateCB = uiWidget->findChild<QCheckBox*>("stateCB");
    m_cityCB = uiWidget->findChild<QCheckBox*>("cityCB");
    m_locationCB = uiWidget->findChild<QCheckBox*>("locationCB");
    m_lim = uiWidget->findChild<QAbstractButton*>("asGPSLIM_button");
    m_fnl = uiWidget->findChild<QAbstractButton*>("asGPSFNL_button");
    m_l_lat = uiWidget->findChild<QLabel*>("labLat");
    m_l_lng = uiWidget->findChild<QLabel*>("labLng");
    m_l_alt = uiWidget->findChild<QLabel*>("labAlt");
    m_l_date = uiWidget->findChild<QLabel*>("labDate");
    m_l_time = uiWidget->findChild<QLabel*>("labTime");
    m_l_status = uiWidget->findChild<QLabel*>("labStat");
    m_l_sats = uiWidget->findChild<QLabel*>("labSat");
    m_l_countryCode = uiWidget->findChild<QLabel*>("labCountryCode");
    m_l_country = uiWidget->findChild<QLabel*>("labCountry");
    m_l_state = uiWidget->findChild<QLabel*>("labState");
    m_l_city = uiWidget->findChild<QLabel*>("labCity");
    m_l_location = uiWidget->findChild<QLabel*>("labLoc");
    m_moveMap = uiWidget->findChild<QAbstractButton*>("moveMap");
    m_setAsStart = uiWidget->findChild<QAbstractButton*>("setAsStart");
    m_home = uiWidget->findChild<QAbstractButton*>("homeButton");

    //GPS Track
    m_t_filename = uiWidget->findChild< QLineEdit * >( "trackFileEdit" );
    m_t_lng = uiWidget->findChild< QLineEdit * >( "trackLonEdit" );
    m_t_lat = uiWidget->findChild< QLineEdit * >( "trackLatEdit" );
    m_t_alt = uiWidget->findChild< QLineEdit * >( "trackAltEdit" );
    m_t_status= uiWidget->findChild< QLabel * >( "trackStatusLabel" );
    m_t_filebutton = uiWidget->findChild< QAbstractButton * >( "trackFileButton" );
    m_t_timezone = uiWidget->findChild< QSpinBox * >( "trackTimeZoneSpinBox" );
    m_t_timeoffset = uiWidget->findChild< QSpinBox * >( "trackTimeOffset" );
    m_t_localTZ = uiWidget->findChild< QCheckBox * >( "trackLocalTZCheck" );
    m_t_tracktime = uiWidget->findChild< QLabel * >( "trackTimeInterval" );
    m_t_tracktime->setText("");
	//GPS Track

    m_cb_bicycle = uiWidget->findChild<QCheckBox*>("cbBicycle");
    m_cb_traffic = uiWidget->findChild<QCheckBox*>("cbTraffic");
    m_cb_weather = uiWidget->findChild<QCheckBox*>("cbWeather");
    m_cb_clouds = uiWidget->findChild<QCheckBox*>("cbClouds");
    m_cb_panoramio = uiWidget->findChild<QCheckBox*>("cbPanoramio");

    m_keepMapPos = uiWidget->findChild<QCheckBox*>("keepMapPos");

    m_coordsCB = uiWidget->findChild<QCheckBox*>("coordsCB");
    m_iptcCB = uiWidget->findChild<QCheckBox*>("iptcCB");

    m_googleCoordinates = uiWidget->findChild<QTextEdit*>("googleCoordinates");
    m_googleRaw = uiWidget->findChild<QTextEdit*>("googleRaw");

    QLabel *lab_configPath = uiWidget->findChild<QLabel*>("configPath");
    lab_configPath->setText(m_configDir);

    m_internalMapPage = new MyWebPage();
    m_externalMapPage = new MyWebPage();

    QGridLayout *layout = (QGridLayout*)(QWidget*)(uiWidget->findChild<QWidget*>("asGPSMapTab"))->layout();
    m_internalView = new InternalWebView(NULL, this);
    layout->addWidget(m_internalView, 0, 0);
    m_internalView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_internalView->updateGeometry();

    initMap(m_internalView, m_internalMapPage, true);
    initMap(m_externalView, m_externalMapPage, false);

    connect(m_reset, SIGNAL( clicked() ), SLOT ( reset() ));
    connect(m_reload, SIGNAL( clicked() ), SLOT ( reload() ));
    connect(m_lim, SIGNAL( clicked() ), SLOT ( geocode() ));
    connect(m_fnl, SIGNAL( clicked() ), SLOT ( reversegeocode() ));
    connect(m_tag, SIGNAL ( clicked() ), SLOT ( tagImage() ));
    connect(m_enable, SIGNAL( toggled(bool) ), SLOT (handleCheckedChange(bool) ));
    connect(m_info, SIGNAL(clicked()), SLOT(displayHelp() ));
    connect(m_coordsCB, SIGNAL( stateChanged(int) ), SLOT( handleCoordsCB(int) ));
    connect(m_iptcCB, SIGNAL( stateChanged(int) ), SLOT( handleIptcCB(int) ) );
    connect(m_xmap, SIGNAL( toggled(bool) ), SLOT( handleXmapChange(bool) ) );

    connect(m_t_filename, SIGNAL( editingFinished() ), SLOT( trackLoad() ) );
    connect(m_t_localTZ, SIGNAL( clicked() ), SLOT( trackGetPos() ) );
    connect(m_t_timezone, SIGNAL( editingFinished() ), SLOT( trackGetPos() ) );
    connect(m_t_timezone, SIGNAL( valueChanged(int) ), SLOT( trackGetPos(int) ) );
    connect(m_t_timeoffset, SIGNAL( editingFinished() ), SLOT( trackGetPos() ) );
    connect(m_t_timeoffset, SIGNAL( valueChanged(int) ), SLOT( trackGetPos(int) ) );
    connect(m_t_filebutton, SIGNAL ( clicked() ), SLOT ( trackFileDialog() ));

    connect(m_cb_bicycle, SIGNAL(clicked()), SLOT(mapLayers()));
    connect(m_cb_traffic, SIGNAL(clicked()), SLOT(mapLayers()));
    connect(m_cb_weather, SIGNAL(clicked()), SLOT(mapLayers()));
    connect(m_cb_clouds, SIGNAL(clicked()), SLOT(mapLayers()));
    connect(m_cb_panoramio, SIGNAL(clicked()), SLOT(mapLayers()));

    m_iptcCB->setCheckState(Qt::PartiallyChecked);
    m_coordsCB->setCheckState(Qt::PartiallyChecked);

    connect(m_latCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsLat(int) ) );
    connect(m_lngCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsLng(int) ) );
    connect(m_altCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsAlt(int) ) );
    connect(m_dateCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsDate(int) ) );
    connect(m_timeCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsTime(int) ) );
    connect(m_statusCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsStatus(int) ) );
    connect(m_satsCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsSats(int) ) );

    connect(m_countryCodeCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsCountryCode(int) ) );
    connect(m_countryCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsCountry(int) ) );
    connect(m_stateCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsState(int) ) );
    connect(m_cityCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsCity(int) ) );
    connect(m_locationCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsLocation(int) ) );

    connect(m_moveMap, SIGNAL( clicked() ), SLOT( updateMap() ));
    connect(m_setAsStart, SIGNAL( clicked() ), SLOT( setAsStartPos() ));
    connect(m_home, SIGNAL( clicked() ), SLOT( home() ));
}

void asGPSplugin::fileSelectorUserCountries() {
    QString mapName = QFileDialog::getOpenFileName(NULL,
        tr("Select your country table"), m_configDir);
    if (mapName != 0) {
        m_countryMap->setText(mapName);
    }
}

void asGPSplugin::countryTableChanged(QString table) {
    m_config->countryTable(table);
    qDebug() << "asGPS: loading user defined country table from" << table;
    m_iso3661.clear();
    m_iso3661.load();
    m_iso3661.load(table);
}

void asGPSplugin::readAndCreateConfigFile() {

    connect(m_openEnabled, SIGNAL( toggled(bool) ), m_config, SLOT( startEnabled(bool) ) );
    connect(m_openExtraMap, SIGNAL( toggled(bool) ), m_config, SLOT( startWithExternalMap(bool) ) );
    connect(m_cc3, SIGNAL( toggled(bool) ), m_config, SLOT( cc3(bool) ) );
    connect(m_center, SIGNAL( toggled(bool) ), m_config, SLOT( centerOnClick(bool) ) );
    connect(m_allIptc, SIGNAL( toggled(bool) ), m_config, SLOT( searchAllIPTC(bool) ) );
    connect(m_checkUpdates, SIGNAL( toggled(bool) ), m_config, SLOT( checkForUpdates(bool) ) );
    connect(m_splitGpsTime, SIGNAL( toggled(bool) ), m_config, SLOT( splitGpsTimestamp(bool) ) );

    connect(m_keepMapPos, SIGNAL( toggled(bool) ), m_config, SLOT( keepMapOnHotnessChange(bool) ) );

    connect(m_countryMap, SIGNAL ( textChanged(QString) ), SLOT ( countryTableChanged(QString) ) );

    connect(m_mapLanguage, SIGNAL ( textChanged(QString) ), m_config, SLOT ( mapLanguage(QString) ) );
    connect(m_mapRegion, SIGNAL ( textChanged(QString) ), m_config, SLOT ( mapRegion(QString) ) );

    connect(m_fileSelect, SIGNAL ( clicked() ), SLOT( fileSelectorUserCountries() ) );

    if (m_config->startEnabled()) {
        m_openEnabled->setChecked(true);
        m_enable->setChecked(true); // map eingeschaltet beim Start von ASP
        QTimer::singleShot(5000, this, SLOT(reload()));
    } else {
        m_openEnabled->setChecked(false);
        m_enable->setChecked(false); // map ausgeschaltet beim Start von ASP
    }

    if (m_config->startWithExternalMap()) {
        m_openExtraMap->setChecked(true);
        m_xmap->setChecked(true);
    } else {
        m_openExtraMap->setChecked(false);
        m_xmap->setChecked(false);
    }

    m_cc3->setChecked(m_config->cc3());
    m_allIptc->setChecked(m_config->searchAllIPTC());
    m_center->setChecked(m_config->centerOnClick());
    m_checkUpdates->setChecked(m_config->checkForUpdates());
    m_splitGpsTime->setChecked(m_config->splitGpsTimestamp());

    m_countryMap->setText(m_config->countryTable());

    m_mapLanguage->setText(m_config->mapLanguage());
    m_mapRegion->setText(m_config->mapRegion());

    m_keepMapPos->setChecked(m_config->keepMapOnHotnessChange());

    m_countryCodeCB->setCheckState((Qt::CheckState)m_config->cbSettingsCountryCode());
    m_countryCB->setCheckState((Qt::CheckState)m_config->cbSettingsCountry());
    m_stateCB->setCheckState((Qt::CheckState)m_config->cbSettingsState());
    m_cityCB->setCheckState((Qt::CheckState)m_config->cbSettingsCity());
    m_locationCB->setCheckState((Qt::CheckState)m_config->cbSettingsLocation());

    m_latCB->setCheckState((Qt::CheckState)m_config->cbSettingsLat());
    m_lngCB->setCheckState((Qt::CheckState)m_config->cbSettingsLng());
    m_altCB->setCheckState((Qt::CheckState)m_config->cbSettingsAlt());
    m_dateCB->setCheckState((Qt::CheckState)m_config->cbSettingsDate());
    m_timeCB->setCheckState((Qt::CheckState)m_config->cbSettingsTime());
    m_statusCB->setCheckState((Qt::CheckState)m_config->cbSettingsStatus());
    m_satsCB->setCheckState((Qt::CheckState)m_config->cbSettingsSats());
}

void asGPSplugin::initMap(QWebView *view, QWebPage *page, bool toolsMap) {
    m_loaded--;
    view->hide();
    connect(view,
                SIGNAL( loadFinished ( bool ) ),
            this,
                SLOT( handleLoadFinished ( bool ) ));
    view->setPage(page);
    page->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    updateMapUrl(view, toolsMap);
    connect(page,
            SIGNAL( linkClicked(QUrl) ),
               this,
            SLOT( openInternalBrowser(QUrl) ));
    connect(page->mainFrame(),
            SIGNAL(javaScriptWindowObjectCleared()),
               this,
            SLOT(populateJavaScriptWindowObject()));
}

void asGPSplugin::updateMapUrl(QWebView *view, bool toolsMap) {
    QString url("qrc:///html/asGPSmap.html");
    url += "?toolsMap=";
    url += ((toolsMap) ? "true" : "false");
    if (m_config->mapLanguage() != "") {
        url += "&language=";
        url += m_config->mapLanguage();
    }
    qDebug() << "asGPS: updateMapUrl =" << url;
    view->setUrl(QUrl(url));
}

void asGPSplugin::webViewError() {
    QMessageBox::information(NULL, tr("Error creating QWebView"), tr("It was not possible to create a QWebView widget."));
}

void asGPSplugin::populateJavaScriptWindowObject() {
    m_internalMapPage->mainFrame()->addToJavaScriptWindowObject(QString("api"), this);
    m_externalMapPage->mainFrame()->addToJavaScriptWindowObject(QString("api"), this);
}

void asGPSplugin::handleHotnessChanged( const PluginImageSettings &options )
{
    Q_UNUSED(options);

    qDebug() << "\n\nasGPSplugin: handleHotnessChanged";

    if (inSettingsChange) {
        qDebug() << "asGPSplugin: hotness ignored";
        return;
    }

    QString merk_lat = m_lat->text();
    QString merk_lng = m_lng->text();
    QString merk_alt = m_alt->text();

    resetIPTC();
    resetGPS();
    resetGoogle();

    if (options.options(0) != NULL) {
        bool ok=true;
        qDebug() << "asGPS: HHC " << options.options(0)->getString(ID_Location, 0, ok) << ok;

        updateUi(options.options(0));
 
		int iopt=m_pHub->optionIdForName("DigitizedDateTime",0);	//needed the photo time for every photo, even if it's not enabled
		photoTime=options.options(0)->getString(iopt,0,ok);

        if (!m_enable->isChecked()) {
            return;
        }

        hideUnhideMarker(merk_lat, merk_lng, merk_alt);

        trackGetPos();

        if (m_autolim) geocode();
        if (m_autofnl) reversegeocode();
        if (m_autotag) {
            qDebug() << "asGPS: would like to autotag - 300ms - then emit the signal...";
            QTimer::singleShot(300, m_tag, SLOT(click()));
        }

    }

    // if we have no GPS info
    if (m_lat->text().isEmpty() && m_lng->text().isEmpty()) {
        if (m_config->keepMapOnHotnessChange()) {
            m_lat->setText(merk_lat);
            m_lng->setText(merk_lng);
            m_alt->setText(merk_alt);
            updateMap();
        } else {
            qDebug() << "asGPS: no GPS go to home pos...";
            home();
        }
    }

}

void asGPSplugin::handleSettingsChanged( const PluginImageSettings &options,  const PluginImageSettings &changed, int layer )
{
    Q_UNUSED(changed);

    qDebug() << "\n\nasGPSplugin: handleSettingsChanged started on layer" << layer;

    // only run in main layer
    if (layer == 0 && options.options(0) != NULL) {
        bool ok;
        qDebug() << "asGPS: HSC " << options.options(0)->getString(ID_Location, 0, ok) << ok;

        QString merk_lat = m_lat->text();
        QString merk_lng = m_lng->text();
        QString merk_alt = m_alt->text();

        updateUi(options.options(0));

        hideUnhideMarker(merk_lat, merk_lng, merk_alt);

    } else {
        if (m_enable->isChecked()) updateMap();
    }

}

void asGPSplugin::hideUnhideMarker(QString merk_lat, QString merk_lng, QString merk_alt) {
    // if image has no lat/lng values
    if (m_config->keepMapOnHotnessChange() && m_lat->text().isEmpty() && m_lng->text().isEmpty()) {
        qDebug() << "asGPS: hideMarker";
        m_internalMapPage->mainFrame()->evaluateJavaScript("hideMarker()");
        m_externalMapPage->mainFrame()->evaluateJavaScript("hideMarker()");
        m_lat->setText(merk_lat);
        m_lng->setText(merk_lng);
        m_alt->setText(merk_alt);
    }
    // if image is tagged with lat/lng
    if (m_config->keepMapOnHotnessChange() && !m_l_lat->text().isEmpty() && !m_l_lng->text().isEmpty()) {
        qDebug() << "asGPS: unhideMarker";
        m_internalMapPage->mainFrame()->evaluateJavaScript("unhideMarker()");
        if (m_xmap->isChecked()) {
            m_externalMapPage->mainFrame()->evaluateJavaScript("unhideMarker()");
        }
    }
}

void asGPSplugin::setStringField(PluginOptionList *options, QLineEdit *field, int optionID) {
    bool ok;
    QString s(options->getString(optionID,0,ok));
    if (ok) field->setText(s);
}

void asGPSplugin::tag(PluginOptionList *options, QLineEdit *field, QCheckBox *cb, QLabel *lab, int optionID) {
    if (!m_enable->isChecked()) return;
    if ((cb == NULL) || (cb->checkState() == Qt::Checked) || ((cb->checkState() == Qt::PartiallyChecked) && (lab->text() == ""))) {
        qDebug() << QString("asGPS: setting option %1 to: '" + field->text() + "'").arg(optionID).toAscii();
        options->setString(optionID, 0, field->text());
        // negative altitudes must be set as negative values, but also the ref has to be set.
        if (optionID == ID_GPSAltitude) {
            if (field->text().at(0) == QChar('-')) {
                options->setInt(ID_GPSAltitudeRef, 0, 1);
            } else {
                options->setInt(ID_GPSAltitudeRef, 0, 0);
            }

        }
    }
}

void asGPSplugin::setStringField(PluginOptionList *options, QLabel *field, int optionID) {
    bool ok;
    QString s(options->getString(optionID,0,ok));
    if (ok)
        field->setText(s);
}

void asGPSplugin::resetIPTC() {
    m_countryCode->setText("");
    m_country->setText("");
    m_state->setText("");
    m_city->setText("");
    m_location->setText("");
}

void asGPSplugin::resetGPS() {
    m_lat->setText("");
    m_lng->setText("");
    m_alt->setText("");
    m_date->setText("");
    m_time->setText("");
    m_status->setText("");
    m_sats->setText("");
}

void asGPSplugin::resetGoogle() {
    m_googleCoordinates->setText("");
    m_googleRaw->setText("");
}

void asGPSplugin::reset() {
    m_edit->setText("");
    resetIPTC();
    resetGPS();
    resetGoogle();
    updateMap();
}

void asGPSplugin::reload() {
    qDebug() << "asGPS: reload";
    if (!m_enable->isChecked()) return;
    resetIPTC();
    resetGPS();
    resetGoogle();
    m_countryCode->setText(m_l_countryCode->text());
    m_country->setText(m_l_country->text());
    m_state->setText(m_l_state->text());
    m_city->setText(m_l_city->text());
    m_location->setText(m_l_location->text());
    m_lat->setText(m_l_lat->text());
    m_lng->setText(m_l_lng->text());
    m_alt->setText(m_l_alt->text());
    m_date->setText(m_l_date->text());
    m_time->setText(m_l_time->text());
    m_status->setText(m_l_status->text());
    m_sats->setText(m_l_sats->text());
    splitDate();
    m_edit->setText(getIPTCconcat());
    QString merk_lat = m_lat->text();
    QString merk_lng = m_lng->text();
    QString merk_alt = m_alt->text();
    if (merk_lat.isEmpty() && merk_lng.isEmpty()) {
        setHomeCoordinates();
    }
    hideUnhideMarker(merk_lat, merk_lng, merk_alt);
    updateMap();
}

void asGPSplugin::fillGoogleRaw(QString rawData) {
    TrackPoint loc(QString(m_l_date->text()).append(" ").append(m_l_time->text()), m_lat->text(), m_lng->text(), m_alt->text());
    QString coords = m_lat->text() + " / " + m_lng->text();
    QStringList formatted = loc.formatAsDegreesMinutesSeconds(4);
    coords += "<br/>" + formatted[0] + " / " + formatted[1];
    m_googleCoordinates->setText(coords.replace("°","&deg;"));
    m_googleRaw->setText(rawData);
}

void asGPSplugin::handleIptcCB(int unused) {
    Q_UNUSED(unused);
    Qt::CheckState state = m_iptcCB->checkState();
    qDebug() << "asGPS: handleIptcCB" << state;
    m_countryCodeCB->setCheckState(state);
    m_countryCB->setCheckState(state);
    m_stateCB->setCheckState(state);
    m_cityCB->setCheckState(state);
    m_locationCB->setCheckState(state);
}

void asGPSplugin::handleCoordsCB(int unused) {
    Q_UNUSED(unused);
    Qt::CheckState state = m_coordsCB->checkState();
    qDebug() << "asGPS: handleCoordsCB" <<state;
    m_latCB->setCheckState(state);
    m_lngCB->setCheckState(state);
    m_altCB->setCheckState(state);
    m_dateCB->setCheckState(state);
    m_timeCB->setCheckState(state);
    m_statusCB->setCheckState(state);
    m_satsCB->setCheckState(state);
}

void asGPSplugin::updateUi(PluginOptionList *options) {
    qDebug() << "asGPS: updateUI";
    setStringField(options, m_l_lat, ID_GPSLatitude);
    setStringField(options, m_l_lng, ID_GPSLongitude);
    setStringField(options, m_l_alt, ID_GPSAltitude);
    setStringField(options, m_l_date, ID_GPSDateStamp);
    setStringField(options, m_l_time, ID_GPSTimeStamp);
    setStringField(options, m_l_status, ID_GPSStatus);
    setStringField(options, m_l_sats, ID_GPSSatellites);
    setStringField(options, m_l_countryCode, ID_CountryCode);
    setStringField(options, m_l_country, ID_Country);
    setStringField(options, m_l_state, ID_State);
    setStringField(options, m_l_city, ID_City);
    setStringField(options, m_l_location, ID_Location);
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: updateUI continued with asGPS enabled";
    setStringField(options, m_lat, ID_GPSLatitude);
    setStringField(options, m_lng, ID_GPSLongitude);
    setStringField(options, m_alt, ID_GPSAltitude);
    setStringField(options, m_date, ID_GPSDateStamp);
    setStringField(options, m_time, ID_GPSTimeStamp);
    setStringField(options, m_status, ID_GPSStatus);
    setStringField(options, m_sats, ID_GPSSatellites);
    setStringField(options, m_countryCode, ID_CountryCode);
    setStringField(options, m_country, ID_Country);
    setStringField(options, m_state, ID_State);
    setStringField(options, m_city, ID_City);
    setStringField(options, m_location, ID_Location);
    splitDate();
    m_edit->setText(getIPTCconcat());
    updateMap();
}

void asGPSplugin::splitDate() {
    if (m_config->splitGpsTimestamp()) {
        QStringList dsts = m_time->text().split(" ");
        if (dsts.length() == 2) {
            m_date->setText(dsts.at(0));
            m_time->setText(dsts.at(1));
        }
    }
}

void asGPSplugin::clearTags() {
    m_l_lat->setText("");
    m_l_lng->setText("");
    m_l_alt->setText("");
    m_l_date->setText("");
    m_l_time->setText("");
    m_l_status->setText("");
    m_l_sats->setText("");
    m_l_countryCode->setText("");
    m_l_country->setText("");
    m_l_state->setText("");
    m_l_city->setText("");
    m_l_location->setText("");
}

void asGPSplugin::tagImage() {
    if (!m_enable->isChecked()) return;
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    bool isCTRL = keyMod.testFlag(Qt::ControlModifier);
    if (isCTRL) {
        m_autotag = !m_autotag;
        if (m_autotag) {
            m_tag->setStyleSheet("QPushButton {background-color: rgb(0, 100, 0); }");
        } else {
            m_tag->setStyleSheet(m_default_button_style);
        }
        qDebug() << "asGPS: changed tag state" << (m_autotag);
    } else {
        qDebug() << "asGPS: beginSettingsChange()";
        inSettingsChange = true;
        PluginOptionList* options = m_pHub->beginSettingsChange("asGPS: GPS & IPTC");
        qDebug() << "asGPS: options =" << options;
        if (options) {
            tag(options, m_lat, m_latCB, m_l_lat, ID_GPSLatitude);
            tag(options, m_lng, m_lngCB, m_l_lng, ID_GPSLongitude);
            tag(options, m_alt, m_altCB, m_l_alt, ID_GPSAltitude);
            tag(options, m_date, m_dateCB, m_l_date, ID_GPSDateStamp);
            tag(options, m_time, m_timeCB, m_l_time, ID_GPSTimeStamp);
            tag(options, m_status, m_statusCB, m_l_status, ID_GPSStatus);
            tag(options, m_sats, m_satsCB, m_l_sats, ID_GPSSatellites);
            tag(options, m_countryCode, m_countryCodeCB, m_l_countryCode, ID_CountryCode);
            tag(options, m_country, m_countryCB, m_l_country, ID_Country);
            tag(options, m_state, m_stateCB, m_l_state, ID_State);
            tag(options, m_city, m_cityCB, m_l_city, ID_City);
            tag(options, m_location, m_locationCB, m_l_location, ID_Location);
            m_pHub->endSettingChange();
            inSettingsChange = false;
            qDebug() << "asGPS: endSettingsChange()";
        }
    }
}

void asGPSplugin::updateMap() {
    qDebug() << "asGPS: updateMap loaded =" << m_loaded; 
    if (m_enable->isChecked()) {
        qDebug() << "asGPS: updateMap with map checked";
        TrackPoint gpsl("", m_lat->text(), m_lng->text(), m_alt->text());
        if (gpsl.lat == 0 && gpsl.lng == 0) {
            if (m_config->keepMapOnHotnessChange()) {
                qDebug() << "asGPS: keeping Map on last Position";
                return;
            } else {
                setHomeCoordinates();
                gpsl = TrackPoint("", m_lat->text(), m_lng->text(), m_alt->text());
            }
        }
        m_internalMapPage->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMapC(" +
            QString("%1").arg(gpsl.lat,0,'f',5) + "," + QString("%1").arg(gpsl.lng,0,'f',5) + ")");
        m_externalMapPage->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMapC(" +
            QString("%1").arg(gpsl.lat,0,'f',5) + "," + QString("%1").arg(gpsl.lng,0,'f',5) + ")");
    }
}

void asGPSplugin::geocode() {
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: geocode" << m_edit->text();
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    bool isCTRL = keyMod.testFlag(Qt::ControlModifier);
    if (isCTRL) {
        m_autolim = !m_autolim;
        if (m_autolim) {
            m_fnl->setStyleSheet(m_default_button_style);
            m_autofnl = false;
            m_lim->setStyleSheet("QPushButton {background-color: rgb(0, 100, 0); }");
        } else {
            m_lim->setStyleSheet(m_default_button_style);
        }
        qDebug() << "asGPS: changed lim state" << (m_autolim);
    } else {
        resetGPS();
        if (m_edit->text().length()==0) {
            m_edit->setText(getIPTCconcat());
        }
        m_internalMapPage->mainFrame()->evaluateJavaScript(
            "codeCoordinatesFrom('" + m_edit->text() +
            "'," + (m_enable->isChecked() ? "true" : "false") +
            ", '" + m_config->mapRegion() + "')");
        m_externalMapPage->mainFrame()->evaluateJavaScript(
            "codeCoordinatesFrom('" + m_edit->text() +
            "'," + (m_enable->isChecked() ? "true" : "false") +
            ", '" + m_config->mapRegion() + "')");
        updateMap();
    }
}

QString asGPSplugin::getIPTCconcat() {
    QString tmp("");
    if (m_allIptc->isChecked()) {
        if (m_countryCode->text().length()>0) tmp.append(m_countryCode->text());
        if (m_country->text().length()>0) tmp.append(", " + m_country->text());
        if (m_state->text().length()>0) tmp.append(", " + m_state->text());
        if (m_city->text().length()>0) tmp.append(", " + m_city->text());
        if (m_location->text().length()>0) tmp.append(", " + m_location->text());
    } else {
        return m_location->text();
    }
    return tmp;
}


void asGPSplugin::reversegeocode() {
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: reversegeocode" << m_lat->text() + "," + m_lng->text();
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    bool isCTRL = keyMod.testFlag(Qt::ControlModifier);
    if (isCTRL) {
        m_autofnl = !m_autofnl;
        if (m_autofnl) {
            m_lim->setStyleSheet(m_default_button_style);
            m_autolim = false;
            m_fnl->setStyleSheet("QPushButton {background-color: rgb(0, 100, 0); }");
        } else {
            m_fnl->setStyleSheet(m_default_button_style);
        }
        qDebug() << "asGPS: changed fnl state" << (m_autofnl);
    } else {
        TrackPoint gpsl("", m_lat->text(), m_lng->text(), m_alt->text());
        resetIPTC();
        QStringList qsl = gpsl.formatAsGoogle(5);
        m_internalMapPage->mainFrame()->evaluateJavaScript("codeAddressFrom('" + qsl.at(0) + "," + qsl.at(1) + "'," + (m_enable->isChecked() ? "true" : "false") + ")");
        m_externalMapPage->mainFrame()->evaluateJavaScript("codeAddressFrom('" + qsl.at(0) + "," + qsl.at(1) + "'," + (m_enable->isChecked() ? "true" : "false") + ")");
        updateMap();
    }
}

void asGPSplugin::setOptionIDs() {
    ID_GPSLatitude = m_pHub->optionIdForName(ON_GPSLatitude,0);
    ID_GPSLongitude = m_pHub->optionIdForName(ON_GPSLongitude,0);
    ID_GPSAltitude = m_pHub->optionIdForName(ON_GPSAltitude,0);
    ID_GPSAltitudeRef = m_pHub->optionIdForName(ON_GPSAltitudeRef,0);
    ID_GPSDateStamp = m_pHub->optionIdForName(ON_GPSDateStamp,0);
    ID_GPSTimeStamp = m_pHub->optionIdForName(ON_GPSTimeStamp,0);
    ID_GPSStatus = m_pHub->optionIdForName(ON_GPSStatus,0);
    ID_GPSSatellites = m_pHub->optionIdForName(ON_GPSSatellites,0);
    ID_Location = m_pHub->optionIdForName(ON_Location,0);
    ID_CountryCode = m_pHub->optionIdForName(ON_CountryCode,0);
    ID_Country = m_pHub->optionIdForName(ON_Country,0);
    ID_State = m_pHub->optionIdForName(ON_State,0);
    ID_City = m_pHub->optionIdForName(ON_City,0);
    ID_Location = m_pHub->optionIdForName(ON_Location,0);
}

void asGPSplugin::marker_click(bool toolsMap)
{
    Q_UNUSED(toolsMap);
    qDebug() << "asGPS: marker clicked";
}

void asGPSplugin::marker_moved(double lat, double lng, double height, bool toolsMap)
{
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: marker moved:" << lat << lng << toolsMap;
    TrackPoint gpsl(QDateTime::currentDateTime(), lat, lng, height);
    QStringList qsl = gpsl.formatAsOption(3);
    m_lat->setText(qsl.at(0));
    m_lng->setText(qsl.at(1));
    m_alt->setText(QString("%1/1").arg(height,0,'f',0));
    m_status->setText("A");
    if (m_autofnl) reversegeocode();
    QWebView *wv = toolsMap ? m_externalView : m_internalView;
    QWebView *ov = toolsMap ? m_internalView : m_externalView;
    wv->page()->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMapC(" +
        QString("%1").arg(lat,0,'f',5) + "," + QString("%1").arg(lng,0,'f',5) + ")");
    if (m_center->isChecked()) {
        ov->page()->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMapC(" +
            QString("%1").arg(lat,0,'f',5) + "," + QString("%1").arg(lng,0,'f',5) + ")");
    }
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    bool isSHIFT = keyMod.testFlag(Qt::ShiftModifier);
    if (isSHIFT) {
        QTimer::singleShot(100, m_tag, SLOT(click()));
    }

}

void asGPSplugin::set_country(QString short_name, QString long_name) {
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: set_country" << long_name << short_name;
    m_country->setText(long_name);
    QString cc3 = m_iso3661.from2to3(short_name);
    if (cc3=="" || !m_cc3->isChecked()) {
        m_countryCode->setText(short_name);
    } else {
        m_countryCode->setText(cc3);
    }
}

void asGPSplugin::set_city(QString short_name, QString long_name) {
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: set_city" << long_name << short_name;
    m_city->setText(long_name);
}

void asGPSplugin::set_state(QString short_name, QString long_name) {
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: set_state" << long_name << short_name;
    m_state->setText(long_name);
}

void asGPSplugin::set_location(QString long_name) {
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: set_location" << long_name;
    m_location->setText(long_name);
}

void asGPSplugin::handleLoadFinished ( bool ok ) {
    qDebug() << "asGPS: load finished" << ok;
    m_loaded++;
    if (m_loaded == 0) {
        readAndCreateConfigFile();
    }
}

void asGPSplugin::handleCheckedChange(bool enabled) {
    qDebug() << "asGPS: enabled change enabled =" << enabled;
    if (enabled) {
        reload();
        m_internalView->show();
        if (m_xmap->isChecked())
            m_externalView->show();
        updateMap();
    } else {
        m_internalView->hide();
        m_externalView->hide();
        resetIPTC();
        resetGPS();
        resetGoogle();
    }
}

void asGPSplugin::handleXmapChange(bool enabled) {
    qDebug() << "asGPS: enabled xmap enabled =" << enabled;
    if (enabled) {
        if (m_enable->isChecked()) {
            if (m_xmap->isChecked())
                m_externalView->show();
//TEST      m_externalView->page()->mainFrame()->evaluateJavaScript("trackView()");
            updateMap();
        }
    } else {
        m_externalView->hide();
    }
}

void asGPSplugin::alert(QString text) {
    qDebug() << "asGPS: alert: " + text;
    QMessageBox::information(NULL,tr("asGPS - Information"), text);
}

void asGPSplugin::openInternalBrowser(QUrl url) {
    qDebug() << "asGPS: openInternalBrowser" << url;
    QWebView *view = new QWebView();
    if (!view) return;
    view->setPage(new MyWebPage());
    view->setUrl(url);
    view->setWindowTitle(tr("AfterShot Pro - asGPS browser window"));
    view->setWindowIcon(view->icon());
    view->show();
}

void asGPSplugin::openExternalBrowser(QUrl url) {
    qDebug() << "asGPS: openExternalBrowser" << url;
    QDesktopServices::openUrl(url);
}

void asGPSplugin::displayHelp() {
    qDebug() << "asGPS: displayHelp";
    QWebView *view = new QWebView();
    if (!view) return;
    view->setPage(new MyWebPage());
    view->setUrl(QUrl(tr("qrc:///html/asGPSinfo_EN.html")));
    view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect( view->page(),
             SIGNAL( linkClicked(QUrl) ),
                this,
             SLOT( openExternalBrowser(QUrl) ));
    view->setWindowTitle("AfterShot Pro - asGPS v"  TARGET_VERSION_STRING);
    view->setWindowIcon(view->icon());
    view->show();
}

void asGPSplugin::trackFileDialog() {
	qDebug() << "asGPS: trackFileDialog";
	QString filename = QFileDialog::getOpenFileName(0, tr("Open GPS track"),
			QString(), tr("Track files (*.gpx *.csv);;All files(*.*)")); //Display file open dialog
	if (filename == "")
		return; //cancel pressed
	m_t_filename->setText(filename);
	trackLoad();
}

void asGPSplugin::trackLoad() {
    if (m_enable->checkState() == false) {
        m_t_status->setText("<B>Enable the plugin to read the track");
        return;
    }
    qDebug() << "asGPS: trackLoad";
    QString filename = m_t_filename->text();
    int res;
    if (track) {
        delete track;
        track = NULL;
    }
    if (!filename.isEmpty()) {
        if (filename.endsWith(".csv", Qt::CaseInsensitive)) {
            track = new GpsCsv(filename);
        } else {
            track = new GpsGpx(filename);
        }
        res = track->parsefile();
        if (res == CGps::FileErr) m_t_status->setText("<B><font color=#FF0000>Error:</font></B> File open problem.");
        if (res == CGps::ParseErr)
            m_t_status->setText("<B><font color=#FF0000>Error:</font></B> Parse problem or empty track.");
        if (res == CGps::OK) {
            qDebug() << "asGPS: track loaded";
            m_t_tracktime->setText(
                track->first().time.toString(TimeDate_DEF) + " UTC ... " + track->last().time.toString(TimeDate_DEF)
                    + " UTC");
            trackGetPos();
        }
    } else {
        m_t_status->setText("no track file loaded.");
        m_t_lat->setText("");
        m_t_lng->setText("");
        m_t_alt->setText("");
        m_t_tracktime->setText("");
    }

    m_internalView->page()->mainFrame()->evaluateJavaScript("trackView()");
    m_externalView->page()->mainFrame()->evaluateJavaScript("trackView()");

    if (track != NULL && !track->isEmpty()) {
        qDebug() << "asGPS: centering map on track start";
        TrackPoint start = track->first();
        m_internalView->page()->mainFrame()->evaluateJavaScript(
            QString("centerMap(%1,%2)").arg(start.lat).arg(start.lng));
        m_externalView->page()->mainFrame()->evaluateJavaScript(
            QString("centerMap(%1,%2)").arg(start.lat).arg(start.lng));
    }
}

void asGPSplugin::trackGetPos(int dummy) {
    Q_UNUSED(dummy);
    if (track == NULL) return;
    if (!track->isLoaded()) return;
    bool localTZ = m_t_localTZ->checkState();
    int tzData = m_t_timezone->value();
    int offset = m_t_timeoffset->value();
    QDateTime time = QDateTime::fromString(photoTime, TimeDate_DEF);
    CGps::ParseResult res = track->searchElement(time, localTZ, tzData, offset);
    if (res != CGps::OK) {
        m_t_status->setText(
            QString("<B><font color=#FF0000>Error:</font></B> Timestamp %1 UTC not found in track file.").arg(
                track->position.time.toString(TimeDate_DEF)));
        m_t_lat->setText("");
        m_t_lng->setText("");
        m_t_alt->setText("");
        return;
    } else {
        m_t_status->setText(
            QString("<B><font color=#00FF00>Success!</font></B> Position at %1 UTC found.").arg(
                track->position.time.toString(TimeDate_DEF)));
    }
    QStringList pos = track->position.formatAsOption(4);
    m_t_lat->setText(pos[0]);
    m_t_lng->setText(pos[1]);
    m_t_alt->setText(pos[2]);
    m_lat->setText(pos[0]);
    m_lng->setText(pos[1]);
    m_alt->setText(pos[2]);
    updateMap();
}

int asGPSplugin::getTrackSize() {
    if (track == NULL) return 0;
    return track->size();
}

QString asGPSplugin::getTrackPoint(int n) {
    if (track == NULL || n >= track->size()) return "";
    TrackPoint tp = track->at(n);
    QString ret = QString("%1:%2:%3:%4").arg(tp.lat, 0, 'f', 8).arg(tp.lng, 0, 'f', 8).arg((int) tp.type).arg(tp.name);
//    qDebug() << "track: " << ret;
    return ret;
}

void asGPSplugin::mapLayers() {
    QString command("mapLayers(");
    command.append(m_cb_bicycle->isChecked() ? "true" : "false");
    command.append(", ");
    command.append(m_cb_traffic->isChecked() ? "true" : "false");
    command.append(", ");
    command.append(m_cb_weather->isChecked() ? "true" : "false");
    command.append(", ");
    command.append(m_cb_clouds->isChecked() ? "true" : "false");
    command.append(", ");
    command.append(m_cb_panoramio->isChecked() ? "true" : "false");
    command.append(")");
    m_internalView->page()->mainFrame()->evaluateJavaScript(command);
    m_externalView->page()->mainFrame()->evaluateJavaScript(command);
}

void asGPSplugin::setAsStartPos() {
    m_config->mapStartPosition(m_lat->text() + ":" + m_lng->text() + ":" + m_alt->text());
}

void asGPSplugin::home() {
    setHomeCoordinates();
    updateMap();
}

void asGPSplugin::setHomeCoordinates() {
    QStringList startPos = m_config->mapStartPosition().split(":");
    if (startPos.length() >= 2) {
        qDebug() << "asGPS: setting configured start position.";
        m_lat->setText(startPos[0]);
        m_lng->setText(startPos[1]);
        if (startPos.length() >= 3) {
            m_alt->setText(startPos[2]);
        }
    }

}
