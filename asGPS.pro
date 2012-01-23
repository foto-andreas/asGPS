######################################################################
# Automatically generated by qmake (2.01a) Wed Mar 3 22:59:29 2010
######################################################################

# TEMPLATE: set to "lib" for the ASP plugin, set to "app" for a cunit test application
TEMPLATE = lib

QT += core
QT += gui
QT += webkit
QT += testlib
QT += network

TARGET = asGPS
VERSION = 1.0.0

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../Plugin
include( ../common/mac.pri )

DEFINES += TARGET_VERSION=\'\"$$VERSION\"\'

# Input
HEADERS += asGPSplugin.h gpsLocation.h Tests.h  WebContents.h WebInfos.h

SOURCES += asGPSplugin.cpp gpsLocation.cpp Tests.cpp WebContents.cpp WebInfos.cpp

RESOURCES += asGPS.qrc

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

FORMS += asGPS.ui

CONFIG(release,debug|release) {
	message( release )

	UI_DIR		=	build/objects/release/ui
	MOC_DIR		=	build/objects/release/moc
	OBJECTS_DIR	=	build/objects/release/obj
	RCC_DIR	    =	build/objects/release/rcc
	UI_HEADERS_DIR = build/objects/release/uih
	UI_SOURCES_DIR = build/objects/release/uisrc
}

unix {
!mac {
   QMAKE_CFLAGS   += -m32 -mfpmath=sse -msse2
   QMAKE_CXXFLAGS += -m32 -mfpmath=sse -msse2 
   QMAKE_LFLAGS   += -m32 -mfpmath=sse -msse2
   QMAKE_LIBS     += -L/usr/lib32
   QMAKE_POST_LINK=./afz
}
}





















