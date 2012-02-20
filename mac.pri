# Mac-only block
macx {
	# Target Mac OS X 10.5 and later & use the 10.6 SDK
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
	QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.6.sdk
	
	# Use GCC 4.0 - later versions of GCC will not work on Mac OS X 10.4
    # QMAKE_CC	= gcc-4.0
    # QMAKE_CXX	= g++-4.0
	
    # Add the Release Xcode configuration - it will now be available in the Target window
    macx-xcode {
		SOURCES += "Release.xcconfig"
	}
	
	# Create a Deploy 'extra' target
	#	This 'extra' target is used to change the hard-coded paths to the Qt 
	#	frameworks from machine-dependent paths to paths relative to the 
	#	executable.  This must be done prior to releasing a plugin.  It is the
	#	last step in preparing a release for download/testing.
	#	Note that 'extra' targets do not work with Xcode projects so in order to
	#	run this step on a Release build you must create a 'Makefile' instead of
	#	an Xcode project.  Do this as follows in the Terminal 
	#	(where <Target Name> is the name of your .pro file):
	#	
	#	$ qmake -spec macx-g++40 <Target Name>.pro
	#	$ make Deploy
	Deploy.target = Deploy
	Deploy.commands = ../../tools/fixplugins.sh lib$${TARGET}.$$QMAKE_EXTENSION_SHLIB
	
	QMAKE_EXTRA_TARGETS += Deploy
}
