TEMPLATE = app
CONFIG += qt \
    qdbus
CONFIG -= thread
QT += network
CODECFORSRC = UTF-8
CODECFORTR = ISO-8859-1
OBJECTS_DIR = build/
MOC_DIR = build/
UI_DIR = build/
TARGET = qnut
FORMS = ipconf.ui \
    devset.ui \
    airset.ui \
    apconf.ui \
    devdet.ui \
    adhoc.ui
TRANSLATIONS = qnut_de.ts \
    qnut_pt_BR.ts
HEADERS += connectionmanager.h \
    constants.h \
    ipconfiguration.h \
    common.h \
    interfacedetailsmodel.h \
    wirelesssettings.h \
    managedapmodel.h \
    environmenttreemodel.h \
    availableapmodel.h \
    accesspointconfig.h \
    environmentdetailsmodel.h \
    dnslistmodel.h \
    ipeditdelegate.h \
    adhocconfig.h \
    devicesettings.h \
    commandlistmodel.h \
    cuidevicemodel.h \
    cuidevice.h \
    cnotificationmanager.h
SOURCES += main.cpp \
    connectionmanager.cpp \
    ipconfiguration.cpp \
    common.cpp \
    interfacedetailsmodel.cpp \
    wirelesssettings.cpp \
    managedapmodel.cpp \
    environmenttreemodel.cpp \
    availableapmodel.cpp \
    accesspointconfig.cpp \
    environmentdetailsmodel.cpp \
    dnslistmodel.cpp \
    ipeditdelegate.cpp \
    adhocconfig.cpp \
    devicesettings.cpp \
    commandlistmodel.cpp \
    cuidevicemodel.cpp \
    cuidevice.cpp \
    cnotificationmanager.cpp
DESTDIR = .
target.path = /usr/bin
iconstarget.path = /usr/share/qnut/icons
iconstarget.files = res/*.png \
    res/qnut.svg \
    res/qnut_small.svg
langtarget.path = /usr/share/qnut/lang
langtarget.files = qnut_*.qm
shortcuttarget.path = /usr/share/applications
shortcuttarget.files = qnut.desktop
INSTALLS += target \
    iconstarget \
    langtarget \
    shortcuttarget
INCLUDEPATH += ..
QMAKE_CXXFLAGS_DEBUG += -pedantic \
    -Wno-long-long
LIBS += -lnutclient \
    -L../libnutclient
