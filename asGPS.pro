######################################################################
# QT project file for asGPS - an AfterShot Pro geocoding plugin
# (c) Andreas Schrell, Wermelskirchen, DE
######################################################################

# set to "lib" for the ASP plugin
TEMPLATE = lib

# Include these lines for build a main application which does the cunit tests
#TEMPLATE = app
#QT += testlib
#HEADERS += Tests.h
#SOURCES += Tests.cpp

# Include default Qt libraries
QT += core
QT += gui

# Include extra Qt libraries for the web views resp. map
QT += webkit
QT += network


# what we build here
TARGET = asGPS
VERSION = 1.0.4

# This is used in the source
DEFINES += TARGET_VERSION=$$VERSION

# everything depends on local path and locale files
DEPENDPATH += . locale

# we are including local header files
INCLUDEPATH += .

# we are including the ASP SDK headers from various locations where they may be
DEPENDPATH += ../Plugin ../SDK/Plugin ../SDK
INCLUDEPATH += ../Plugin ../SDK/Plugin ../SDK

# our header files
HEADERS += asGPSplugin.h gpsLocation.h WebContents.h WebInfos.h \
    TargetVersion.h \
    iso3166.h \
    ConfigFile.h \
    ConfigurationMapper.h

# our source files
SOURCES += asGPSplugin.cpp gpsLocation.cpp WebContents.cpp WebInfos.cpp \
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
    asGPSmap_EN.html \
    asGPSmap_DE.html \
    asGPSmap_NL.html \
    asGPSmap_JA.html \
    asGPSmap_IT.html \
    asGPSmap_FR.html \
    iso3166.txt

# the user interface file
FORMS += asGPS.ui

# we are building a release
CONFIG(release,debug|release) {
	message( release )
	UI_DIR		=	build/objects/release/ui
	MOC_DIR		=	build/objects/release/moc
	OBJECTS_DIR	=	build/objects/release/obj
	RCC_DIR	    =	build/objects/release/rcc
	UI_HEADERS_DIR = build/objects/release/uih
	UI_SOURCES_DIR = build/objects/release/uisrc
}

# include mac build defines only on mac. these are from the SDK examples
mac {
    include( mac.pri )
    include( Release.xcconfig )
}

# on unix, but not on OS/X we pack the plugin file and create the documentation.
# feel free to disable the QMAKE_POST_LINKs if you pack with PluginZipper and
# do not create a new documentation.
unix {
!mac {
   QMAKE_CFLAGS   += -m32 -mfpmath=sse -msse2
   QMAKE_CXXFLAGS += -m32 -mfpmath=sse -msse2 
   QMAKE_LFLAGS   += -m32 -mfpmath=sse -msse2
   QMAKE_LIBS     += -L/usr/lib32
   QMAKE_POST_LINK += "./afz $$VERSION"
   QMAKE_POST_LINK += "; doxygen"
}
}

# SSE2, faster coder over smaller, full optimization
windows {
    QMAKE_CXXFLAGS += /arch:SSE2 /Oi /Ot
}





