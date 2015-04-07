######################################################################
# QT project file for asGPS - an AfterShot Pro geocoding plugin
# (c) Andreas Schrell, Wermelskirchen, DE
######################################################################

# what we build here (plugin name and version)
TARGET = asGPS
VERSION = 1.4.0
IDENTIFIER = de.schrell.asGPS
AUTHOR = Andreas Schrell
SITE = http://fotos.schrell.de

include ( ../PluginDefaults/PluginDefaults.pri )

# Include these lines for build a main application which does the cunit tests
#TEMPLATE = app
#QT += testlib
#HEADERS += Tests.h
#SOURCES += Tests.cpp

# Include extra Qt libraries for the web views resp. map
QT += webkitwidgets
QT += network
QT += xml

# our header files
HEADERS += \
    cgps.h \
    gpscsv.h \
    gpsgpx.h \
    asGPSplugin.h \
    iso3166.h \
    ConfigurationMapper.h \
    trackpoint.h \
    tracklist.h \
    ../PluginTools/WebInfos.h \
    ../PluginTools/WebContents.h \
    ../PluginTools/ToolData.h \
    ../PluginTools/ConfigFile.h \
    ../PluginTools/TargetVersion.h \
    MyWebPage.h

# our source files
SOURCES += \
    cgps.cpp \
    gpscsv.cpp \
    gpsgpx.cpp \
    asGPSplugin.cpp \
    iso3166.cpp \
    ConfigurationMapper.cpp \
    trackpoint.cpp \
    tracklist.cpp \
    ../PluginTools/WebInfos.cpp \
    ../PluginTools/WebContents.cpp \
    ../PluginTools/ConfigFile.cpp \
    ../PluginTools/ToolData.cpp

# our resource file with html pages and images
RESOURCES += asGPS.qrc

# the other files we pack in the resources
OTHER_FILES += asGPS.pro \
    asGPSinfo_DE.html \
    asGPSinfo_EN.html \
    style.css \
    asGPS.js \
    iso3166.txt \
    gup.js \
    asGPSmap.html \
    ../PluginDefaults/PluginDefaults.pri \
    trackView.js \
    locale/*.ts \
    asGPS.afpxml \
    asGPSol.js \
    trackViewOl.js

# the user interface file
FORMS += asGPS.ui

# translations
TRANSLATIONS += locale/asGPS_en.ts
TRANSLATIONS += locale/asGPS_de.ts
TRANSLATIONS += locale/asGPS_nl.ts
TRANSLATIONS += locale/asGPS_fr.ts
TRANSLATIONS += locale/asGPS_it.ts
TRANSLATIONS += locale/asGPS_ja.ts

unix {
!mac {
# private extra targets here on my linux box
QMAKE_POST_LINK += "echo 'extras...'"

# we create the source documentation
QMAKE_POST_LINK += "; doxygen"

# locale files
QMAKE_POST_LINK += "; lrelease $${TARGET}.pro"

# strip the lib
QMAKE_POST_LINK += "; strip 'lib$${TARGET}.so.$${VERSION}'"

# we pack our plugin - I hate PZ
QMAKE_POST_LINK += "; ../PluginDefaults/afz '$$TARGET' '$$VERSION' '$$IDENTIFIER' '$$AUTHOR' '$$SITE' $$EX "
QMAKE_POST_LINK += " 'asGPS GPS & IPTC tagging.xmp' 'asGPS IPTC tagging.xmp'"
}
}



























