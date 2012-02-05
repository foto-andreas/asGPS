######################################################################
# QT project file for asGPS - an AfterShot Pro geocoding plugin
# (c) Andreas Schrell, Wermelskirchen, DE
######################################################################

# TEMPLATE: set to "lib" for the ASP plugin, set to "app" for a cunit test application
TEMPLATE = lib

# Include default Qt libraries
QT += core
QT += gui

# Include extra Qt libraries for the web views resp. map
QT += webkit
QT += network

# Include extra Qt libraries für the cunit tests
QT += testlib

# what we build here
TARGET = asGPS
VERSION = 1.0.1

# This is used in the source
DEFINES += TARGET_VERSION=$$VERSION

# everything depends on local path, we ignore the ASP plugin includes here
DEPENDPATH += .

# we are including local header files
INCLUDEPATH += .

# we are including the ASP SDK headers from various locations where they may be
INCLUDEPATH += ../Plugin ../SDK/Plugin ../SDK

# include mac build defines only on mac. these are from the SDK examples
mac {
    include( mac.pri )
    include( Release.xcconfig )
}

# our header files
HEADERS += asGPSplugin.h gpsLocation.h Tests.h  WebContents.h WebInfos.h \
    TargetVersion.h

# our source files
SOURCES += asGPSplugin.cpp gpsLocation.cpp Tests.cpp WebContents.cpp WebInfos.cpp

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
    asGPSmap_JA.htm \
    asGPSmap_NL.htm \
    asGPSmap_IT.htm \
    asGPSmap_NL.html \
    asGPSmap_JA.html \
    asGPSmap_IT.html \
    asGPSmap_FR.html

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
