#ifndef LIBNUT_LIBNUT_CLI_H
#define LIBNUT_LIBNUT_CLI_H

#include <QObject>
#include <QList>
#include <QHostAddress>
#include "libnut_types.h"
#include "libnut_server_proxy.h"
/*
Benötigte Informationen:
device liste : /device_name/
    environment_liste
        
*/

namespace libnut {
    class CDeviceManager;

    class CDevice;
    class CEnvironment;
    class CInterface;

    typedef QList<CDevice *> CDeviceList;
    typedef QList<CEnvironment *> CEnvironmentList;
    typedef QList<CInterface *> CInterfaceList;
};

namespace libnut {
    class CDeviceManager : public QObject {
        Q_OBJECT
    public:
        CDeviceList devices;
        
        CDeviceManager(QObject * parent);
    signals:
        void deviceAdded(CDevice * device);
        void deviceRemoved(CDevice * device); //nach entfernen aus der liste aber vor dem löschen
    };

    class CDevice : public QObject {
        Q_OBJECT
    public:
        libnut_DeviceProperties properties;
        CEnvironmentList environments;
        
        CDevice(QObject * parent);
    public slots:
        void enable();
        void disable();

    signals:
        void environmentChangedActive(int current, int previous); //parameter current hinzugefügt
        void environmentsUpdated(); //parameter device entfernt
        void stateChanged(bool state);
    };
    
    class CEnvironment : public QObject {
        Q_OBJECT
    public:
        libnut_EnvironmentProperties properties;
        libnut_SelectConfig currentSelection;
        QList<libnut_SelectConfig> selectStatements;
        CInterfaceList interfaces;
        
        CEnvironment(CDevice * parent);
    public slots:
        void enter(); //zuvor activate()
    signals:
        void stateChanged(bool state); //hinzugefügt
        void interfacesUpdated();
    };
    
    class CInterface : public QObject {
        Q_OBJECT
    public:
        libnut_InterfaceProperties properties;
        CInterface(QObject * parent);
    public slots:
        void activate();
        void deactivate();
        void setIP(QHostAddress * address);
        void setNetmask(QHostAddress * address);
        void setGateway(QHostAddress * address);
    signals:
        void stateChanged(bool state); //zuvor activeStateChanged()
    };
};

#endif
