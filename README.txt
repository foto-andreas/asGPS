
asGPS - Geotagging Plugin for AfterShotPro

Read the license information in LICENSE.txt. By the use of this code or the plugin you agree to these licenses.

ASP SDK:
- Download the AfterShotPro SDK and unpack it

asGPS:
- extract asGPS info a folder if you crated a snapshot or use
  git clone http://schrell.de/asGPS.git
  to clone it into a new directory asGPS
- change the INCLUDEPATH and mac include in asGPS.pro for your needs. The mac includes comes with the SDK.

compiling in QtSDK:
- open the asGPS.pro and create a release version with Qt 4.7.3. For me also 4.7.4 works.

compiling at the command prompt:
- call
  qmake
- call
  make

Pack:
- use the PluginZipper for packaging

