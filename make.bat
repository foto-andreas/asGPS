rem include files
set INCLUDE=c:\Programme\Microsoft Visual Studio 9.0\VC\INCLUDE;C:\Programme\Microsoft SDKs\Windows\v6.0A\include

rem libs
set LIB=c:\Programme\Microsoft Visual Studio 9.0\VC\LIB;C:\Programme\Microsoft SDKs\Windows\v6.0A\lib

rem paths
set PATH=%PATH%;C:\Programme\Git\cmd
set PATH=%PATH%;C:\Programme\Microsoft SDKs\Windows\v6.0A\bin
set PATH=%PATH%;C:\Programme\Microsoft Visual Studio 9.0\VC\bin;C:\Programme\Microsoft Visual Studio 9.0\VC\lib
set PATH=%PATH%;C:\Programme\Microsoft Visual Studio 9.0\Common7\IDE
set PATH=%PATH%;C:\Programme\QtSDK\Desktop\Qt\4.7.4\msvc2008\bin
set PATH=%PATH%;C:\Programme\QtSDK\QtCreator\bin

rem create Makefile.Release
qmake CONFIG+=Release

rem make all clean
jom clean -j12 -f Makefile.Release

rem combile release version
jom -j 12 -f Makefile.Release
