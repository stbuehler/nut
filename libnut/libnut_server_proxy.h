/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -a libnut_server_adaptor.h -p libnut_server_proxy.cpp libnut_server.xml
 *
 * dbusxml2cpp is Copyright (C) 2006 Trolltech ASA. All rights reserved.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
*  beep. need to edit. edid ist da und editet.
 */

#ifndef LIBNUT_SERVER_PROXY_H
#define LIBNUT_SERVER_PROXY_H

#include <QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <common/dbus.h>
#include <common/types.h>
#include <QMetaType>
namespace libnut {
/*
 * Proxy class for interface DBus.Device
 */
class DBusDeviceInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return NUT_DBUS_URL ".Device"; }

public:
    DBusDeviceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~DBusDeviceInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<bool> disable()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("disable"), argumentList);
    }

    inline QDBusReply<bool> enable()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("enable"), argumentList);
    }

    inline QDBusReply<QList<QDBusObjectPath> > getEnvironments()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getEnvironments"), argumentList);
    }
    inline QDBusReply<libnut::libnut_DeviceProperties> getProperties() {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getProperties"), argumentList);
    }
    inline QDBusReply<void> setEnvironment(QDBusObjectPath envpath) {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(envpath);
        return callWithArgumentList(QDBus::Block, QLatin1String("setEnvironment"), argumentList);
    }
    inline QDBusReply<QList<libnut_wlanScanresult> > getwlanScan() {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getwlanScan"), argumentList);
    }
    inline QDBusReply<QDBusObjectPath> addwlanEnvironment(libnut_wlanNetworkProperties netprops) {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(netprops);
        return callWithArgumentList(QDBus::Block, QLatin1String("addwlanEnvironment"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void environmentChangedActive(const QDBusObjectPath &newenv);
    void environmentAdded(const QDBusObjectPath &path);
    void environmentRemoved(const QDBusObjectPath &path);
    void stateChanged(const bool &state);
};

/*
 * Proxy class for interface DBus.DeviceManager
 */
class DBusDeviceManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return NUT_DBUS_URL ".DeviceManager"; }

public:
    DBusDeviceManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~DBusDeviceManagerInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<QList<QDBusObjectPath> > getDeviceList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getDeviceList"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void deviceAdded(const QDBusObjectPath &objectpath);
    void deviceRemoved(const QDBusObjectPath &objectpath);
};

/*
 * Proxy class for interface DBus.Environment
 */
class DBusEnvironmentInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return NUT_DBUS_URL ".Environment"; }

public:
    DBusEnvironmentInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~DBusEnvironmentInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<QList<QDBusObjectPath> > getInterfaces()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getInterfaces"), argumentList);
    }

    inline QDBusReply<QList<libnut::libnut_SelectConfig> > getSelectConfig()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getSelectConfig"), argumentList);
    }

    inline QDBusReply<libnut::libnut_SelectConfig> getCurrentSelection()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getCurrentSelection"), argumentList);
    }
    inline QDBusReply<libnut::libnut_EnvironmentProperties> getProperties()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getProperties"), argumentList);
    }
    inline QDBusReply<QDBusObjectPath> addInterface(libnut_InterfaceProperties prop, bool state) {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(prop) << qVariantFromValue(state);
        return callWithArgumentList(QDBus::Block, QLatin1String("addInterface"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void interfacesUpdated();
};

/*
 * Proxy class for interface DBus.Interface
 */
class DBusInterfaceInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return NUT_DBUS_URL ".Interface"; }

public:
    DBusInterfaceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~DBusInterfaceInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<void> activate()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("activate"), argumentList);
    }

    inline QDBusReply<void> deactivate()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("deactivate"), argumentList);
    }

    inline QDBusReply<libnut::libnut_InterfaceProperties> getProperties()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getProperties"), argumentList);
    }

    inline QDBusReply<void> setIP(uint HostAddress)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(HostAddress);
        return callWithArgumentList(QDBus::Block, QLatin1String("setIP"), argumentList);
    }

    inline QDBusReply<void> setGateway(uint Gateway)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(Gateway);
        return callWithArgumentList(QDBus::Block, QLatin1String("setGateway"), argumentList);
    }

    inline QDBusReply<void> setNetmask(uint Netmask)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(Netmask);
        return callWithArgumentList(QDBus::Block, QLatin1String("setNetmask"), argumentList);
    }

    inline QDBusReply<void> setNetmask()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("setDynamic"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void StateChanged(const bool &state);
};
/*
namespace DBus {
  typedef ::DBusDeviceInterface Device;
  typedef DBus::DBusDeviceManagerInterface DeviceManager;
  typedef DBus::DBusEnvironmentInterface Environment;
  typedef DBus::DBusInterfaceInterface Interface;
}*/
}
#endif
