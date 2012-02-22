#include "ConfigurationMapper.h"

#include <QDebug>
#include <QString>

#include "ConfigFile.h"

static const char *STARTENABLED = "startEnabled";
static const char *STARTWITHEXT = "startWithExternalMap";
static const char *USETHREELETTER = "useThreeLetterCountryCodes";
static const char *SEARCHALLIPTC = "searchWithAllIptcFields";
static const char *CENTERONCLICK = "centerMapOnClick";
static const char *CHECKFORUPDATES = "checkForUpdates";
static const char *SPLITGPSTIMESTAMP = "splitGpsTimestamp";
static const char *HOTNESSWORKAROUND = "hotnessWorkaround";

static const char *COUNTRYTABLE = "userCountryTable";
static const char *MAPLANGUAGE = "mapLanguage";
static const char *MAPREGION = "mapRegion";

static const char *CBLAT = "checkBoxLat";
static const char *CBLNG = "checkBoxLng";
static const char *CBALT = "checkBoxAlt";
static const char *CBDATE = "checkBoxDate";
static const char *CBTIME = "checkBoxTime";
static const char *CBSTATUS = "checkBoxStatus";
static const char *CBSATS = "checkBoxSats";
static const char *CBCC = "checkBoxCountryCode";
static const char *CBCOUNTRY = "checkBoxCountry";
static const char *CBSTATE = "checkBoxState";
static const char *CBCITY = "checkBoxCity";
static const char *CBLOC = "checkBoxLocation";

ConfigurationMapper::ConfigurationMapper(QString fileName) {
    qDebug() << "asGPS: ConfigurationMapper";
    m_cf = new ConfigFile(fileName);
    m_cf->autoWrite(true);
}

bool ConfigurationMapper::getBool(const char *key, bool def = false) {
    QString val = m_cf->getValue(key);
    if (val == NULL) return def;
    return val == "true";
}

QString ConfigurationMapper::getString(const char *key, QString def = "") {
    QString val = m_cf->getValue(key);
    if (val == NULL) return def;
    return val;
}

int ConfigurationMapper::getInt(const char *key, int def = 0) {
    QString val = m_cf->getValue(key);
    if (val == NULL) return def;
    return val.toInt();
}

bool ConfigurationMapper::startEnabled() {
    return getBool(STARTENABLED);
}

void ConfigurationMapper::startEnabled(bool val) {
    m_cf->setValue(STARTENABLED, val ? "true" : "false" );
}

bool ConfigurationMapper::startWithExternalMap() {
    return getBool(STARTWITHEXT);
}

void ConfigurationMapper::startWithExternalMap(bool val) {
    m_cf->setValue(STARTWITHEXT, val ? "true" : "false" );
}

bool ConfigurationMapper::cc3() {
    return getBool(USETHREELETTER);
}

void ConfigurationMapper::cc3(bool val) {
    m_cf->setValue(USETHREELETTER, val ? "true" : "false" );
}

bool ConfigurationMapper::searchAllIPTC() {
    return getBool(SEARCHALLIPTC);
}

void ConfigurationMapper::searchAllIPTC(bool val) {
    m_cf->setValue(SEARCHALLIPTC, val ? "true" : "false" );
}

bool ConfigurationMapper::centerOnClick() {
    return getBool(CENTERONCLICK);
}

void ConfigurationMapper::centerOnClick(bool val) {
    m_cf->setValue(CENTERONCLICK, val ? "true" : "false" );
}

bool ConfigurationMapper::checkForUpdates() {
    return getBool(CHECKFORUPDATES, true);
}

void ConfigurationMapper::checkForUpdates(bool val) {
    m_cf->setValue(CHECKFORUPDATES, val ? "true" : "false" );
}

bool ConfigurationMapper::splitGpsTimestamp() {
    return getBool(SPLITGPSTIMESTAMP, false);
}

void ConfigurationMapper::splitGpsTimestamp(bool val) {
    m_cf->setValue(SPLITGPSTIMESTAMP, val ? "true" : "false" );
}

bool ConfigurationMapper::hotnessWorkaround() {
    return getBool(HOTNESSWORKAROUND, false);
}

void ConfigurationMapper::hotnessWorkaround(bool val) {
    m_cf->setValue(HOTNESSWORKAROUND, val ? "true" : "false" );
}

QString ConfigurationMapper::countryTable() {
    return getString(COUNTRYTABLE);
}

void ConfigurationMapper::countryTable(QString cT) {
    m_cf->setValue(COUNTRYTABLE, cT);
}

QString ConfigurationMapper::mapLanguage() {
    return getString(MAPLANGUAGE, "");
}

void ConfigurationMapper::mapLanguage(QString language) {
    m_cf->setValue(MAPLANGUAGE, language);
}

QString ConfigurationMapper::mapRegion() {
    return getString(MAPREGION, "");
}

void ConfigurationMapper::mapRegion(QString region) {
    m_cf->setValue(MAPREGION, region);
}

int ConfigurationMapper::cbSettingsLat() {
    return getInt(CBLAT, 1);
}

void ConfigurationMapper::cbSettingsLat(int settings) {
    m_cf->setValue(CBLAT, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsLng() {
    return getInt(CBLNG, 1);
}

void ConfigurationMapper::cbSettingsLng(int settings) {
    m_cf->setValue(CBLNG, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsAlt() {
    return getInt(CBALT, 1);
}

void ConfigurationMapper::cbSettingsAlt(int settings) {
    m_cf->setValue(CBALT, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsDate() {
    return getInt(CBDATE, 1);
}

void ConfigurationMapper::cbSettingsDate(int settings) {
    m_cf->setValue(CBDATE, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsTime() {
    return getInt(CBTIME, 1);
}

void ConfigurationMapper::cbSettingsTime(int settings) {
    m_cf->setValue(CBTIME, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsStatus() {
    return getInt(CBSTATUS, 1);
}

void ConfigurationMapper::cbSettingsStatus(int settings) {
    m_cf->setValue(CBSTATUS, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsSats() {
    return getInt(CBSATS, 1);
}

void ConfigurationMapper::cbSettingsSats(int settings) {
    m_cf->setValue(CBSATS, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsCountryCode() {
    return getInt(CBCC, 1);
}

void ConfigurationMapper::cbSettingsCountryCode(int settings) {
    m_cf->setValue(CBCC, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsCountry() {
    return getInt(CBCOUNTRY, 1);
}

void ConfigurationMapper::cbSettingsCountry(int settings) {
    m_cf->setValue(CBCOUNTRY, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsState() {
    return getInt(CBSTATE, 1);
}

void ConfigurationMapper::cbSettingsState(int settings) {
    m_cf->setValue(CBSTATE, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsCity() {
    return getInt(CBCITY, 1);
}

void ConfigurationMapper::cbSettingsCity(int settings) {
    m_cf->setValue(CBCITY, QString("%1").arg(settings));
}

int ConfigurationMapper::cbSettingsLocation() {
    return getInt(CBLOC, 1);
}

void ConfigurationMapper::cbSettingsLocation(int settings) {
    m_cf->setValue(CBLOC, QString("%1").arg(settings));
}


