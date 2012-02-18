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
static const char *COUNTRYTABLE = "userCountryTable";
static const char *MAPLANGUAGE = "mapLanguage";

ConfigurationMapper::ConfigurationMapper(QString fileName) {
    qDebug() << "asGPS: ConfigurationMapper";
    m_cf = new ConfigFile(fileName);
    m_cf->autoWrite(true);
}

bool ConfigurationMapper::getBool(const char *key) {
    QString val = m_cf->getValue(key);
    if (val == "") {
        m_cf->setValue(key, "false");
    }
    return val == "true";
}

QString ConfigurationMapper::getString(const char *key) {
    QString val = m_cf->getValue(key);
    if (val == "") {
        m_cf->setValue(key, "");
    }
    return val;
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
    return getBool(CHECKFORUPDATES);
}

void ConfigurationMapper::checkForUpdates(bool val) {
    m_cf->setValue(CHECKFORUPDATES, val ? "true" : "false" );
}

QString ConfigurationMapper::countryTable() {
    return getString(COUNTRYTABLE);
}

void ConfigurationMapper::countryTable(QString cT) {
    m_cf->setValue(COUNTRYTABLE, cT);
}

QString ConfigurationMapper::mapLanguage() {
    return getString(MAPLANGUAGE);
}

void ConfigurationMapper::mapLanguage(QString language) {
    m_cf->setValue(MAPLANGUAGE, language);
}

