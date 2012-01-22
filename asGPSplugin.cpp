#include "asGPSplugin.h"

#include "PluginHub.h"
#include "PluginDependency.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"
#include "PluginData.h"

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


#include "gpsLocation.h"

#include "WebInfos.h"

extern "C" BIBBLE_API BaseB5Plugin *b5plugin() { return new asGPSplugin; }

class MyWebPage : public QWebPage
{
    virtual QString userAgentForUrl(const QUrl& url) const {
        Q_UNUSED(url);
        return "Chrome/1.0";
    }

};


bool asGPSplugin::init(PluginHub *hub, int id, int groupId, const QString &)
{
    qDebug() << "asGPSplugin::init";
    m_pHub = hub;
    m_id = id;
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

    m_pHub->startPluginData("asGPS:TestData");
    m_pHub->startPluginData("de.schrell.asGPS:TestData");

    m_webInfos = new WebInfos("de.schrell.asGPS", "8");

    connect(m_webInfos,
            SIGNAL(ready()),
            SLOT(webInfosReady()));

    m_webInfos->fetch();

    return b;
}

void asGPSplugin::webInfosReady() {
    qDebug() << "asGPS: web infos ready:" << m_webInfos->identifier() << m_webInfos->version();
    if (m_webInfos->isWebNewer()) {
        QString text = QString("There is a newer version of %1 available. "
                               "It is version %2. You are running %3. "
                               "You can download it under the following url: <a href='%4'>%4</a>")
                        .arg(m_webInfos->name(), m_webInfos->version(), TARGET_VERSION, m_webInfos->link());
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
    uiWidget->setWindowTitle(uiWidget->windowTitle() + " (" + TARGET_VERSION + ")");
    m_enable = uiWidget->findChild<QCheckBox*>("asGPSEnabled_checker");
    m_enable->setChecked(false); // map ausgeschaltet beim Start von ASP
    m_view = uiWidget->findChild<QWebView*>("asGPSWebView");
    m_view->setGeometry(QRect(0,0,250,250));
    m_view->adjustSize();
    m_edit = uiWidget->findChild<QLineEdit*>("asGPSText_edit");
    m_reset = uiWidget->findChild<QAbstractButton*>("asGPSReset_button");
    m_tag = uiWidget->findChild<QAbstractButton*>("asGPSTag_button");
    m_info = uiWidget->findChild<QAbstractButton*>("asGPSAbout_button");

    m_lat = uiWidget->findChild<QLineEdit*>("latLineEdit");
    m_lon = uiWidget->findChild<QLineEdit*>("lonLineEdit");
    m_alt = uiWidget->findChild<QLineEdit*>("altLineEdit");
    m_date = uiWidget->findChild<QLineEdit*>("dateLineEdit");
    m_time = uiWidget->findChild<QLineEdit*>("timeLineEdit");
    m_stats = uiWidget->findChild<QLineEdit*>("statsLineEdit");
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
    m_statsCB = uiWidget->findChild<QCheckBox*>("statusCB");
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
    m_l_stats = uiWidget->findChild<QLabel*>("labStat");
    m_l_sats = uiWidget->findChild<QLabel*>("labSat");
    m_l_countryCode = uiWidget->findChild<QLabel*>("labCountryCode");
    m_l_country = uiWidget->findChild<QLabel*>("labCountry");
    m_l_state = uiWidget->findChild<QLabel*>("labState");
    m_l_city = uiWidget->findChild<QLabel*>("labCity");
    m_l_location = uiWidget->findChild<QLabel*>("labLoc");

    m_coordsCB = uiWidget->findChild<QCheckBox*>("coordsCB");
    m_iptcCB = uiWidget->findChild<QCheckBox*>("iptcCB");

    connect(m_reset, SIGNAL( clicked() ), SLOT ( reset() ));
    connect(m_lim, SIGNAL( clicked() ), SLOT ( geocode() ));
    connect(m_fnl, SIGNAL( clicked() ), SLOT ( reversegeocode() ));
    connect(m_tag, SIGNAL ( clicked() ), SLOT ( tagImage() ));
    connect(m_enable, SIGNAL( toggled(bool) ), SLOT (handleCheckedChange(bool) ));

    connect(m_info, SIGNAL(clicked()), SLOT(displayHelp() ));

    connect(m_coordsCB, SIGNAL( stateChanged(int) ), SLOT( handleCoordsCB(int) ));
    connect(m_iptcCB, SIGNAL( stateChanged(int) ), SLOT( handleIptcCB(int) ));

    m_view->hide();

    connect( m_view,
                  SIGNAL( loadFinished ( bool ) ),
             this,
                  SLOT( handleLoadFinished ( bool ) ));

    connect( m_view,
                  SIGNAL( loadStarted () ),
             this,
                  SLOT( handleLoadStarted () ));

    m_view->setPage(new MyWebPage());
    m_view->setUrl(QUrl("qrc:///html/asGPSmap.html"));
    m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    connect( m_view->page(),
             SIGNAL( linkClicked(QUrl) ),
                this,
             SLOT( openInternalBrowser(QUrl) ));

    m_iptcCB->setCheckState(Qt::PartiallyChecked);
    m_coordsCB->setCheckState(Qt::PartiallyChecked);

    updateMap();
}

void asGPSplugin::populateJavaScriptWindowObject() {
    m_view->page()->mainFrame()->addToJavaScriptWindowObject(QString("api"), this);
}

void asGPSplugin::handleHotnessChanged( const PluginImageSettings &options )
{
    Q_UNUSED(options);

    qDebug() << "asGPSplugin::handleHotnessChanged";

    //  reset the controls
    reset();

    m_pHub->beginSettingsChange("HELPER");
    m_pHub->endSettingChange();
//    bool ok;
//    optionsNew->setString(ID_Location, 0, options->getString(ID_Location, 0, ok));
//    if (options.options(0) != NULL) {
//        updateUi(options.options(0));
//    }

}

void asGPSplugin::handleSettingsChanged( const PluginImageSettings &options,  const PluginImageSettings &changed, int layer )
{
    Q_UNUSED(changed);
    qDebug() << "asGPSplugin::handleSettingsChanged started" << layer;
    // only run in main layer
    if (layer == 0 && options.options(0) != NULL) {
        updateUi(options.options(0));
    }
}

void asGPSplugin::setStringField(PluginOptionList *options, QLineEdit *field, int optionID) {
    bool ok;
    QString s(options->getString(optionID,0,ok));
    if (ok) field->setText(s);
}

void asGPSplugin::tag(PluginOptionList *options, QLineEdit *field, QCheckBox *cb, QLabel *lab, int optionID) {
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
    m_stats->setText("");
    m_sats->setText("");
}

void asGPSplugin::reset() {
    m_edit->setText("");
    resetIPTC();
    resetGPS();
}

void asGPSplugin::handleIptcCB(int) {
    Qt::CheckState state = m_iptcCB->checkState();
    qDebug() << "asGPS: hanleIptcCB" << state;
    m_countryCodeCB->setCheckState(state);
    m_countryCB->setCheckState(state);
    m_stateCB->setCheckState(state);
    m_cityCB->setCheckState(state);
    m_locationCB->setCheckState(state);
}

void asGPSplugin::handleCoordsCB(int) {
    Qt::CheckState state = m_coordsCB->checkState();
    qDebug() << "asGPS: hanleCoordsCB" <<state;
    m_latCB->setCheckState(state);
    m_lonCB->setCheckState(state);
    m_altCB->setCheckState(state);
    m_dateCB->setCheckState(state);
    m_timeCB->setCheckState(state);
    m_statsCB->setCheckState(state);
    m_satsCB->setCheckState(state);
}

void asGPSplugin::updateUi(PluginOptionList *options) {
    qDebug() << "asGPS: updateUI";
    setStringField(options, m_lat, ID_GPSLatitude);
    setStringField(options, m_lon, ID_GPSLongitude);
    setStringField(options, m_alt, ID_GPSAltitude);
    setStringField(options, m_date, ID_GPSDateStamp);
    setStringField(options, m_time, ID_GPSTimeStamp);
    setStringField(options, m_stats, ID_GPSStatus);
    setStringField(options, m_sats, ID_GPSSatellites);
    setStringField(options, m_countryCode, ID_CountryCode);
    setStringField(options, m_country, ID_Country);
    setStringField(options, m_state, ID_State);
    setStringField(options, m_city, ID_City);
    setStringField(options, m_location, ID_Location);
    setStringField(options, m_edit, ID_Location);
    setStringField(options, m_l_lat, ID_GPSLatitude);
    setStringField(options, m_l_lon, ID_GPSLongitude);
    setStringField(options, m_l_alt, ID_GPSAltitude);
    setStringField(options, m_l_date, ID_GPSDateStamp);
    setStringField(options, m_l_time, ID_GPSTimeStamp);
    setStringField(options, m_l_stats, ID_GPSStatus);
    setStringField(options, m_l_sats, ID_GPSSatellites);
    setStringField(options, m_l_countryCode, ID_CountryCode);
    setStringField(options, m_l_country, ID_Country);
    setStringField(options, m_l_state, ID_State);
    setStringField(options, m_l_city, ID_City);
    setStringField(options, m_l_location, ID_Location);
    updateMap();
}

void asGPSplugin::tagImage() {
    qDebug() << "asGPS: tag Image GPS & IPTC";
    PluginOptionList* options = m_pHub->beginSettingsChange("GPS & IPTC");
    qDebug() << options;
    if (options) {
        tag(options, m_lat, m_latCB, m_l_lat, ID_GPSLatitude);
        tag(options, m_lon, m_lonCB, m_l_lon, ID_GPSLongitude);
        tag(options, m_alt, m_altCB, m_l_alt, ID_GPSAltitude);
        tag(options, m_date, m_dateCB, m_l_date, ID_GPSDateStamp);
        tag(options, m_time, m_timeCB, m_l_time, ID_GPSTimeStamp);
        tag(options, m_stats, m_statsCB, m_l_stats, ID_GPSStatus);
        tag(options, m_sats, m_satsCB, m_l_sats, ID_GPSSatellites);
        tag(options, m_countryCode, m_countryCodeCB, m_l_countryCode, ID_CountryCode);
        tag(options, m_country, m_countryCB, m_l_country, ID_Country);
        tag(options, m_state, m_stateCB, m_l_state, ID_State);
        tag(options, m_city, m_cityCB, m_l_city, ID_City);
        tag(options, m_location, m_locationCB, m_l_location, ID_Location);
        m_pHub->endSettingChange();
    }
}

void asGPSplugin::updateMap() {
      if (m_enable->isChecked()) {
          qDebug() << "asGPS: updateMap with map checked";

          if (m_view->layout() != NULL) m_view->layout()->activate();

          gpsLocation gpsl(m_lat->text(), m_lon->text());

          m_view->page()->mainFrame()->evaluateJavaScript("centerAndMarkOnlyMap(" +
              QString("%1").arg(gpsl.getLat(),0,'f',5) + "," + QString("%1").arg(gpsl.getLng(),0,'f',5) + ")");
      }
      connect(m_view->page()->mainFrame(),
                  SIGNAL(javaScriptWindowObjectCleared()),
              this,
                  SLOT(populateJavaScriptWindowObject()));
}

void asGPSplugin::geocode() {
    qDebug() << "asGPS: geocode" << m_edit->text();
    resetGPS();
    if (m_edit->text().length()==0) {
        QString tmp("");
        if (m_countryCode->text().length()>0) tmp.append(m_countryCode->text() + ", ");
        if (m_country->text().length()>0)tmp.append(m_country->text() + ", ");
        if (m_city->text().length()>0)tmp.append(m_city->text());
        m_edit->setText(tmp);
    }
    m_view->page()->mainFrame()->evaluateJavaScript("codeCoordinatesFrom('" + m_edit->text() + "'," + (m_enable->isChecked() ? "true" : "false") + ")");
}

void asGPSplugin::reversegeocode() {
    qDebug() << "asGPS: reversegeocode" << m_lat->text() + "," + m_lon->text();
    gpsLocation gpsl(m_lat->text(), m_lon->text());
    resetIPTC();
    QStringList qsl = gpsl.formatAsGoogle(5);
    m_view->page()->mainFrame()->evaluateJavaScript("codeAddressFrom('" + qsl.at(0) + "," + qsl.at(1) + "'," + (m_enable->isChecked() ? "true" : "false") + ")");
}

void asGPSplugin::handlePositionFound(double lat, double lng) {
    qDebug() << "asGPS: position = " << lat << "/" << lng;
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

void asGPSplugin::marker_click()
{
    qDebug() << "asGPS: marker clicked";
}

void asGPSplugin::marker_moved(double lat, double lng)
{
    qDebug() << "asGPS: marker moved:" <<lat << lng;
    gpsLocation gpsl(lat,lng);
    QStringList qsl = gpsl.formatAsOption(3);
    m_lat->setText(qsl.at(0));
    m_lon->setText(qsl.at(1));
    m_stats->setText("A");
}

void asGPSplugin::set_country(QString short_name, QString long_name) {
    qDebug() << "asGPS: set_country" << long_name << short_name;
    m_country->setText(long_name);
    m_countryCode->setText(short_name);
}

void asGPSplugin::set_city(QString short_name, QString long_name) {
    qDebug() << "asGPS: set_city" << long_name << short_name;
    m_city->setText(long_name);
}

void asGPSplugin::set_state(QString short_name, QString long_name) {
    qDebug() << "asGPS: set_state" << long_name << short_name;
    m_state->setText(long_name);
}

void asGPSplugin::set_location(QString long_name) {
    qDebug() << "asGPS: set_location" << long_name;
    m_location->setText(long_name);
}

void asGPSplugin::handleLoadFinished ( bool ok) {
    m_view->adjustSize();
    qDebug() << "asGPS: load finished" << ok;
}

void asGPSplugin::handleLoadStarted () {
    qDebug() << "asGPS: load started...";
}

void asGPSplugin::handleCheckedChange(bool enabled) {
    qDebug() << "asGPS: enabled change enabled =" << enabled;
    if (enabled) {
        m_view->show();
        updateMap();
        m_view->hide();
        m_view->show();
        updateMap();
    } else {
        m_view->hide();
    }
}

void asGPSplugin::alert(QString text) {
    qDebug() << "asGPS: alert: " + text;
    QMessageBox::information(NULL,"asGPS - Information", text);
}

void asGPSplugin::openInternalBrowser(QUrl url) {
    qDebug() << "asGPS: openInternalBrowser" << url;
    QWebView *view = new QWebView();
    view->setPage(new MyWebPage());
    view->setUrl(url);
    view->setWindowTitle("AfterShot Pro - asGPS browser window");
    view->setWindowIcon(view->icon());
    view->show();
}

void asGPSplugin::openExternalBrowser(QUrl url) {
    qDebug() << "asGPS: openExternalBrowser" << url;
    QDesktopServices::openUrl(url);
}

void asGPSplugin::displayHelp() {
    QWebView *view = new QWebView;
    view->setPage(new MyWebPage());
    view->setUrl(QUrl("qrc:///html/asGPSinfo_EN.html"));
    view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect( view->page(),
             SIGNAL( linkClicked(QUrl) ),
                this,
             SLOT( openExternalBrowser(QUrl) ));
    view->setWindowTitle("AfterShot Pro - asGPS browser window");
    view->setWindowIcon(view->icon());
    view->show();
}
