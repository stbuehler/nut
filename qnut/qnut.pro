
TEMPLATE = app
CONFIG += qt qdbus
QT += network

CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1

OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = ui/

TARGET = qnut

FORMS = connman.ui \
 ipconf.ui \
 scrset.ui \
 airset.ui \
 apconf.ui \
 devdet.ui \
 adhoc.ui
TRANSLATIONS = qnut_de.ts

HEADERS += connectionmanager.h trayicon.h \
 constants.h \
 overviewmodel.h \
 devicedetails.h \
 ipconfiguration.h \
 common.h \
 scriptsettings.h \
 interfacedetailsmodel.h \
 wirelesssettings.h \
 managedapmodel.h \
 environmenttreemodel.h \
 availableapmodel.h \
 accesspointconfig.h \
 environmentdetailsmodel.h \
 dnslistmodel.h \
 ipeditdelegate.h \
 adhocconfig.h
SOURCES += main.cpp connectionmanager.cpp trayicon.cpp \
 overviewmodel.cpp \
 devicedetails.cpp \
 ipconfiguration.cpp \
 common.cpp \
 scriptsettings.cpp \
 interfacedetailsmodel.cpp \
 wirelesssettings.cpp \
 managedapmodel.cpp \
 environmenttreemodel.cpp \
 availableapmodel.cpp \
 accesspointconfig.cpp \
 environmentdetailsmodel.cpp \
 dnslistmodel.cpp \
 ipeditdelegate.cpp \
 adhocconfig.cpp
DESTDIR = .

target.path = /usr/bin
iconstarget.path = /usr/share/qnut/icons
iconstarget.files = res/*.png
langtarget.path = /usr/share/qnut/lang
langtarget.files = qnut_*.qm
langtarget.extra = lrelease qnut.pro -nounfinished
shortcuttarget.path = /usr/share/applications
shortcuttarget.files = qnut.desktop

INSTALLS += target iconstarget langtarget shortcuttarget

INCLUDEPATH += ..

LIBS += ../libnutcommon/libnutcommon.a \
../libnutclient/libnutclient.a \
../libnutwireless/libnutwireless.a \
-L/lib \
-liw
TARGETDEPS += ../libnutcommon/libnutcommon.a \
../libnutclient/libnutclient.a \
../libnutwireless/libnutwireless.a

FORMS -= devopt.ui

