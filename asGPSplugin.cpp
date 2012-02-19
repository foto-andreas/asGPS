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

#include "TargetVersion.h"
#include "gpsLocation.h"
#include "WebInfos.h"
#include "ConfigurationMapper.h"

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


bool asGPSplugin::init(PluginHub *hub, int id, int groupId, const QString &)
{
    qDebug() << "asGPSplugin::init";
    m_pHub = hub;
    m_pluginId = id;
    m_groupId = groupId;
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
        m_webInfos = new WebInfos("de.schrell.asGPS", "8");
        connect(m_webInfos,
                SIGNAL(ready()),
                SLOT(webInfosReady()));

        m_webInfos->fetch();
    }

    m_iso3661.load();

    return b;
}

void asGPSplugin::webInfosReady() {
    qDebug() << "asGPS: web infos ready:" << m_webInfos->identifier() << m_webInfos->version();
    if (m_webInfos->isWebNewer()) {
        QString text = QString(tr("There is a newer version of %1 available. "
                               "It is version %2. You are running %3. "
                               "You can download it under the following url: <a href='%4'>%4</a>"))
                        .arg(m_webInfos->name(), m_webInfos->version(), TARGET_VERSION_STRING, m_webInfos->link());
        QMessageBox::information(NULL, m_webInfos->name(), text);
    }
}

PluginDependency *asGPSplugin::createDependency(const QString &name)
{
    Q_UNUSED(name);
    qDebug() << "asGPSplugin::createDependency";
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
    m_mapLanguage = uiWidget->findChild<QComboBox*>("mapLanguage");

    m_enable = uiWidget->findChild<QCheckBox*>("enabled");

    m_autotag = false;
    m_autolim = false;
    m_autofnl = false;
    m_internalView = uiWidget->findChild<QWebView*>("asGPSWebView");
    m_externalView = new QWebView();
    m_externalView->setWindowTitle(tr("AfterShot Pro - asGPS map window"));
    m_externalView->setWindowIcon(m_externalView->icon());
    m_externalView->setWindowFlags( m_externalView->windowFlags() & ~Qt::WindowCloseButtonHint);
    m_edit = uiWidget->findChild<QLineEdit*>("asGPSText_edit");
    m_reset = uiWidget->findChild<QAbstractButton*>("asGPSReset_button");
    m_reload = uiWidget->findChild<QAbstractButton*>("asGPSReload_button");
    m_tag = uiWidget->findChild<QAbstractButton*>("asGPSTag_button");
    m_default_button_style = m_tag->styleSheet();
    m_info = uiWidget->findChild<QAbstractButton*>("asGPSAbout_button");

    m_lat = uiWidget->findChild<QLineEdit*>("latLineEdit");
    m_lon = uiWidget->findChild<QLineEdit*>("lonLineEdit");
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
    m_lonCB = uiWidget->findChild<QCheckBox*>("lonCB");
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
    m_l_lon = uiWidget->findChild<QLabel*>("labLng");
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

    m_coordsCB = uiWidget->findChild<QCheckBox*>("coordsCB");
    m_iptcCB = uiWidget->findChild<QCheckBox*>("iptcCB");

    m_xmap = uiWidget->findChild<QCheckBox*>("xMap");

    m_googleCoordinates = uiWidget->findChild<QTextEdit*>("googleCoordinates");
    m_googleRaw = uiWidget->findChild<QTextEdit*>("googleRaw");

    QLabel *lab_configPath = uiWidget->findChild<QLabel*>("configPath");
    lab_configPath->setText(m_configDir);

    m_internalMapPage = new MyWebPage();
    m_externalMapPage = new MyWebPage();

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
    connect(m_iptcCB, SIGNAL( stateChanged(int) ), SLOT( handleIptcCB(int) ));
    connect(m_xmap, SIGNAL( toggled(bool) ), SLOT( handleXmapChange(bool) ) );

    m_iptcCB->setCheckState(Qt::PartiallyChecked);
    m_coordsCB->setCheckState(Qt::PartiallyChecked);

    connect(m_latCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsLat(int) ) );
    connect(m_lonCB, SIGNAL( stateChanged(int) ), m_config, SLOT(cbSettingsLng(int) ) );
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

    readAndCreateConfigFile();

    // TEST
    // QWebView *wv = new QWebView;
    // wv->setUrl(QUrl("http://khm0.googleapis.com/kh?v=102&hl=en&x=198&y=371&z=10&token=26514"));
    // wv->show();

}

void asGPSplugin::fileSelectorUserCountries() {
    QString mapName = QFileDialog::getOpenFileName(NULL,
        tr("Select your country table"), m_configDir);
    if (mapName != 0) {
        m_countryMap->setText(mapName);
    }
}

void asGPSplugin::countryTableChanged(QString table) {
    m_countryMap->blockSignals(true);
    m_config->countryTable(table);
    m_countryMap->blockSignals(false);
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

    connect(m_countryMap, SIGNAL ( textChanged(QString) ), SLOT ( countryTableChanged(QString) ) );
    connect(m_mapLanguage, SIGNAL ( currentIndexChanged(QString) ), m_config, SLOT ( mapLanguage(QString) ) );

    connect(m_fileSelect, SIGNAL ( clicked() ), SLOT( fileSelectorUserCountries() ) );

    if (m_config->startEnabled()) {
        m_openEnabled->setChecked(true);
        m_enable->setChecked(true); // map eingeschaltet beim Start von ASP
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

    if (m_config->cc3()) {
        m_cc3->setChecked(true);
    } else {
        m_cc3->setChecked(false);
    }

    if (m_config->searchAllIPTC()) {
        m_allIptc->setChecked(true);
    } else {
        m_allIptc->setChecked(false);
    }

    if (m_config->centerOnClick()) {
        m_center->setChecked(true);
    } else {
        m_center->setChecked(false);
    }

    if (m_config->checkForUpdates()) {
        m_checkUpdates->setChecked(true);
    } else {
        m_checkUpdates->setChecked(false);
    }

    if (m_config->splitGpsTimestamp()) {
        m_splitGpsTime->setChecked(true);
    } else {
        m_splitGpsTime->setChecked(false);
    }

    m_countryMap->setText(m_config->countryTable());

    m_mapLanguage->setCurrentIndex(m_mapLanguage->findText(m_config->mapLanguage()));


    m_countryCodeCB->setCheckState((Qt::CheckState)m_config->cbSettingsCountryCode());
    m_countryCB->setCheckState((Qt::CheckState)m_config->cbSettingsCountry());
    m_stateCB->setCheckState((Qt::CheckState)m_config->cbSettingsState());
    m_cityCB->setCheckState((Qt::CheckState)m_config->cbSettingsCity());
    m_locationCB->setCheckState((Qt::CheckState)m_config->cbSettingsLocation());

    m_latCB->setCheckState((Qt::CheckState)m_config->cbSettingsLat());
    m_lonCB->setCheckState((Qt::CheckState)m_config->cbSettingsLng());
    m_altCB->setCheckState((Qt::CheckState)m_config->cbSettingsAlt());
    m_dateCB->setCheckState((Qt::CheckState)m_config->cbSettingsDate());
    m_timeCB->setCheckState((Qt::CheckState)m_config->cbSettingsTime());
    m_statusCB->setCheckState((Qt::CheckState)m_config->cbSettingsStatus());
    m_satsCB->setCheckState((Qt::CheckState)m_config->cbSettingsSats());
}

void asGPSplugin::initMap(QWebView * view, QWebPage * page, bool toolsMap) {
    view->hide();

    connect( view,
                  SIGNAL( loadFinished ( bool ) ),
             this,
                  SLOT( handleLoadFinished ( bool ) ));

    view->setPage(page);
    if (m_config->mapLanguage() != "") {
        view->setUrl(QUrl("qrc:///html/asGPSmap_"
           + m_config->mapLanguage() + ".html?toolsMap=" + ((toolsMap) ? "true" : "false")));
    } else {
        view->setUrl(QUrl(tr("qrc:///html/asGPSmap_EN.html") + "?toolsMap=" + ((toolsMap) ? "true" : "false")));
    }
    qDebug() << "asGPS map url =" << view->url();
    page->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    connect( page,
             SIGNAL( linkClicked(QUrl) ),
                this,
             SLOT( openInternalBrowser(QUrl) ));
    connect(page->mainFrame(),
              SIGNAL(javaScriptWindowObjectCleared()),
          this,
              SLOT(populateJavaScriptWindowObject()));

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
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPSplugin::handleHotnessChanged";
    reset();

//    if (m_pHub->beginSettingsChange("asGPS hotness helper")) {
//        m_pHub->endSettingChange();
//    }

    if (options.options(0) != NULL) {
        updateUi(options.options(0));
        if (m_autolim) geocode();
        if (m_autofnl) reversegeocode();
    }

    if (m_enable->isChecked()) {
        m_internalView->hide();
        m_internalView->show();
    }

}

void asGPSplugin::handleSettingsChanged( const PluginImageSettings &options,  const PluginImageSettings &changed, int layer )
{
    Q_UNUSED(changed);
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPSplugin::handleSettingsChanged started" << layer;
    // only run in main layer
    if (layer == 0 && options.options(0) != NULL) {
        updateUi(options.options(0));
    } else {
        updateMap();
    }
}

void asGPSplugin::setStringField(PluginOptionList *options, QLineEdit *field, int optionID) {
    bool ok;
    QString s(options->getString(optionID,0,ok));
    if (ok) field->setText(s);
}

void asGPSplugin::tag(PluginOptionList *options, QLineEdit *field, QCheckBox *cb, QLabel *lab, int optionID) {
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: tag";
    if ((cb == NULL) || (cb->checkState() == Qt::Checked) || ((cb->checkState() == Qt::PartiallyChecked) && (lab->text() == ""))) {
        qDebug() << QString("asGPS: setting option %1 to: '" + field->text() + "'").arg(optionID).toAscii();
        options->setString(optionID, 0, field->text());
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
    m_lon->setText("");
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
    if (!m_enable->isChecked()) return;
    reset();
    if (m_pHub->beginSettingsChange("asGPS reload helper")) {
        m_pHub->endSettingChange();
    }
}

void asGPSplugin::fillGoogleRaw(QString rawData) {
    gpsLocation loc(m_lat->text(), m_lon->text());
    QString coords = m_lat->text() + " / " + m_lon->text();
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
    m_lonCB->setCheckState(state);
    m_altCB->setCheckState(state);
    m_dateCB->setCheckState(state);
    m_timeCB->setCheckState(state);
    m_statusCB->setCheckState(state);
    m_satsCB->setCheckState(state);
}

void asGPSplugin::updateUi(PluginOptionList *options) {
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: updateUI";
    setStringField(options, m_l_lat, ID_GPSLatitude);
    setStringField(options, m_l_lon, ID_GPSLongitude);
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
    setStringField(options, m_lat, ID_GPSLatitude);
    setStringField(options, m_lon, ID_GPSLongitude);
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
    if (m_config->splitGpsTimestamp()) {
        QStringList dsts = m_time->text().split(" ");
        if (dsts.length() == 2) {
            m_date->setText(dsts.at(0));
            m_time->setText(dsts.at(1));
        }
    }
    m_edit->setText(getIPTCconcat());
    updateMap();
}

void asGPSplugin::clearTags() {
    m_l_lat->setText("");
    m_l_lon->setText("");
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
        qDebug() << "asGPS: tag Image GPS & IPTC";
        PluginOptionList* options = m_pHub->beginSettingsChange("GPS & IPTC");
        qDebug() << options;
        if (options) {
            tag(options, m_lat, m_latCB, m_l_lat, ID_GPSLatitude);
            tag(options, m_lon, m_lonCB, m_l_lon, ID_GPSLongitude);
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
        }
    }
}

void asGPSplugin::updateMap() {
    if (m_enable->isChecked()) {
        qDebug() << "asGPS: updateMap with map checked";
        gpsLocation gpsl(m_lat->text(), m_lon->text());
        if (m_internalView->layout() != NULL) m_internalView->layout()->activate();
        if (m_externalView->layout() != NULL) m_externalView->layout()->activate();
        m_internalMapPage->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMap(" +
            QString("%1").arg(gpsl.getLat(),0,'f',5) + "," + QString("%1").arg(gpsl.getLng(),0,'f',5) + ")");
        if (m_xmap->isChecked()) {
            m_externalMapPage->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMap(" +
                QString("%1").arg(gpsl.getLat(),0,'f',5) + "," + QString("%1").arg(gpsl.getLng(),0,'f',5) + ")");
        }
    }
}

void asGPSplugin::geocode() {
    if (!m_enable->isChecked()) return;
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    bool isCTRL = keyMod.testFlag(Qt::ControlModifier);
    if (isCTRL) {
        m_autolim = !m_autolim;
        if (m_autolim) {
//            m_fnl->setStyleSheet(m_default_button_style);
//            m_autofnl = false;
            m_lim->setStyleSheet("QPushButton {background-color: rgb(0, 100, 0); }");
        } else {
            m_lim->setStyleSheet(m_default_button_style);
        }
        qDebug() << "asGPS: changed lim state" << (m_autolim);
    } else {
        qDebug() << "asGPS: geocode" << m_edit->text();
        resetGPS();
        if (m_edit->text().length()==0) {
            m_edit->setText(getIPTCconcat());
        }
        if (m_internalView) {
            m_internalMapPage->mainFrame()->evaluateJavaScript("codeCoordinatesFrom('" + m_edit->text() + "'," + (m_enable->isChecked() ? "true" : "false") + ")");
        }
        if (m_xmap->isChecked())  {
            m_externalMapPage->mainFrame()->evaluateJavaScript("codeCoordinatesFrom('" + m_edit->text() + "'," + (m_enable->isChecked() ? "true" : "false") + ")");
        }
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
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers ();
    bool isCTRL = keyMod.testFlag(Qt::ControlModifier);
    if (isCTRL) {
        m_autofnl = !m_autofnl;
        if (m_autofnl) {
//            m_lim->setStyleSheet(m_default_button_style);
//            m_autolim = false;
            m_fnl->setStyleSheet("QPushButton {background-color: rgb(0, 100, 0); }");
        } else {
            m_fnl->setStyleSheet(m_default_button_style);
        }
        qDebug() << "asGPS: changed fnl state" << (m_autofnl);
    } else {
        qDebug() << "asGPS: reversegeocode" << m_lat->text() + "," + m_lon->text();
        gpsLocation gpsl(m_lat->text(), m_lon->text());
        resetIPTC();
        QStringList qsl = gpsl.formatAsGoogle(5);
        if (m_internalMapPage) {
            m_internalMapPage->mainFrame()->evaluateJavaScript("codeAddressFrom('" + qsl.at(0) + "," + qsl.at(1) + "'," + (m_enable->isChecked() ? "true" : "false") + ")");
        }
        if (m_xmap->isChecked()) {
            m_externalMapPage->mainFrame()->evaluateJavaScript("codeAddressFrom('" + qsl.at(0) + "," + qsl.at(1) + "'," + (m_enable->isChecked() ? "true" : "false") + ")");
        }
        updateMap();
    }
}

void asGPSplugin::setOptionIDs() {
    ID_GPSLatitude = m_pHub->optionIdForName(ON_GPSLatitude,0);
    ID_GPSLongitude = m_pHub->optionIdForName(ON_GPSLongitude,0);
    ID_GPSAltitude = m_pHub->optionIdForName(ON_GPSAltitude,0);
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

void asGPSplugin::marker_moved(double lat, double lng, bool toolsMap)
{
    if (!m_enable->isChecked()) return;
    qDebug() << "asGPS: marker moved:" <<lat << lng << toolsMap;
    gpsLocation gpsl(lat,lng);
    QStringList qsl = gpsl.formatAsOption(3);
    m_lat->setText(qsl.at(0));
    m_lon->setText(qsl.at(1));
    m_status->setText("A");
    QWebView *wv = toolsMap ? m_externalView : m_internalView;
    QWebView *ov = toolsMap ? m_internalView : m_externalView;
    if (wv->layout() != NULL)
        wv->layout()->activate();
    wv->page()->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMap(" +
        QString("%1").arg(lat,0,'f',5) + "," + QString("%1").arg(lng,0,'f',5) + ")");
    if (m_center->isChecked()) {
        ov->page()->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMap(" +
            QString("%1").arg(lat,0,'f',5) + "," + QString("%1").arg(lng,0,'f',5) + ")");
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

void asGPSplugin::autoTag() {
    if (!m_enable->isChecked()) return;
    if (m_autotag) {
        qDebug() << "asGPS: autotag";
        tagImage();
    }
}

void asGPSplugin::handleLoadFinished ( bool ok ) {
    qDebug() << "asGPS: load finished" << ok;
}

void asGPSplugin::handleCheckedChange(bool enabled) {
    qDebug() << "asGPS: enabled change enabled =" << enabled;
    if (enabled) {
        reload();
        m_internalView->show();
        if (m_xmap->isChecked()) m_externalView->show();
        updateMap();
    } else {
        m_internalView->hide();
        if (m_xmap->isChecked()) m_externalView->hide();
        reset();
        clearTags();
    }
}

void asGPSplugin::handleXmapChange(bool enabled) {
    qDebug() << "asGPS: enabled xmap enabled =" << enabled;
    if (enabled) {
        if (m_enable->isChecked()) {
            m_externalView->show();
            updateMap();
        }
    } else {
        m_externalView->hide();
    }
}

void asGPSplugin::alert(QString text) {
    qDebug() << "asGPS: alert: " + text;
    QMessageBox::information(NULL,"asGPS - Information", text);
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
