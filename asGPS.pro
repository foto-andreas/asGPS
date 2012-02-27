######################################################################
# QT project file for asGPS - an AfterShot Pro geocoding plugin
# (c) Andreas Schrell, Wermelskirchen, DE
######################################################################

# what we build here (plugin name and version)
TARGET = asGPS
VERSION = 1.1.3

include ( ../PluginDefaults/PluginDefaults.pri )

# Include these lines for build a main application which does the cunit tests
#TEMPLATE = app
#QT += testlib
#HEADERS += Tests.h
#SOURCES += Tests.cpp

# Include extra Qt libraries for the web views resp. map
QT += webkit
QT += network

# our header files
HEADERS += \
    asGPSplugin.h \
    gpsLocation.h \
    WebContents.h \
    WebInfos.h \
    iso3166.h \
    ConfigFile.h \
    ConfigurationMapper.h

# our source files
SOURCES += \
    asGPSplugin.cpp \
    gpsLocation.cpp \
    WebContents.cpp \
    WebInfos.cpp \
    iso3166.cpp \
    ConfigFile.cpp \
    ConfigurationMapper.cpp

# our resource file with html pages and images
RESOURCES += asGPS.qrc

# the other files we pack in the resources
OTHER_FILES += \
    asGPSinfo_DE.html \
    asGPSinfo_EN.html \
    style.css \
    asGPS.js \
    iso3166.txt \
    gup.js \
    asGPSmap.html \
    PluginDefaults.pri

# the user interface file
FORMS += asGPS.ui

unix {
!mac {
# private extra targets here on my linux box
QMAKE_POST_LINK += "echo 'extras...'"

# we create the source documentation
QMAKE_POST_LINK += "; doxygen"

# we pack our plugin - I hate PZ
QMAKE_POST_LINK += "; ./afz '$$TARGET' '$$VERSION' 'asGPS GPS & IPTC tagging.xmp' 'asGPS IPTC tagging.xmp'
}
}
