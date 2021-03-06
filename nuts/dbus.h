#ifndef NUTSDBUS_H
#define NUTSDBUS_H

#include <QDBusConnection>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QHostAddress>
#include <QTimerEvent>

#include "libnutcommon/common.h"

namespace nuts {
	class DBusDeviceManager;
	class DBusDevice;
	class DBusEnvironment;
	class DBusInterface;
	class DBusInterface_IPv4;
	#ifdef IPv6
	class DBusInterface_IPv6;
	#endif
}

#include "device.h"

namespace nuts {
	
	class DBusDeviceManager: public QDBusAbstractAdaptor {
		Q_OBJECT
		Q_CLASSINFO("D-Bus Interface", "de.unistuttgart.nut" ".DeviceManager")
		private:
			QDBusConnection m_dbusConnection;
			DeviceManager * m_devmgr;
			QHash<QString, DBusDevice *> m_dbusDevices;
			static const QString m_dbusPath, m_dbusDevicesPath;
			int m_timerId;
			libnutcommon::CDBusMonitor m_dbusMonitor;
			bool registered;

			void initTree();
			void timerEvent(QTimerEvent *event);
			
			
			/** Registers DeviceManager+subtree with dbus **/
			void registerAll();
			/** Unregisters DeviceManager+subtree with dbus **/
			void unregisterAll();
			/**Sets a new dbus connection for all subtrees **/
			void setAllDBusConnection(QDBusConnection * connection);
		private slots:
			void devAdded(QString devName, Device *dev);
			void devRemoved(QString devName, Device *dev);
			
			friend int mainApp(int argc, char* argv[]);
			friend class DeviceManager;

			void stopDBus();
			void dbusStopped();
			void dbusStarted();
		public:
			DBusDeviceManager(DeviceManager *devmgr);
			virtual ~DBusDeviceManager();
	
		public slots:
			QList<QDBusObjectPath> getDeviceList();
			QStringList getDeviceNames();

			
			bool createBridge(QString name);
			bool destroyBridge(QDBusObjectPath devicePath);
			bool destroyBridge(qint32 deviceId);
			bool addToBridge(QDBusObjectPath bridge, QList<QDBusObjectPath> devicePaths);
			bool addToBridge(qint32 bridgeId, QList<qint32> deviceIds);
			bool removeFromBridge(QDBusObjectPath bridge, QList<QDBusObjectPath> devicePaths);
			bool removeFromBridge(qint32 bridgeId, QList<qint32> deviceIds);

		signals:
			void deviceAdded(const QDBusObjectPath &objectpath);
			void deviceRemoved(const QDBusObjectPath &objectpath);
			void deviceAdded(const QString &devname);
			void deviceRemoved(const QString &devname);
	};
	
	class DBusDevice: public QDBusAbstractAdaptor {
		Q_OBJECT
		Q_CLASSINFO("D-Bus Interface", "de.unistuttgart.nut" ".Device")
		private:
			Device *m_device;
			QDBusConnection *m_dbusConnection;
			QList<DBusEnvironment* > m_dbusEnvironments;
			QString m_dbusPath;
			libnutcommon::DeviceProperties m_dbusProperties;
			int m_activeEnvironment;
			bool registered;
		
		private slots:
			void stateChanged(libnutcommon::DeviceState newState, libnutcommon::DeviceState oldState);
			void environmentChanged(int newEnvironment);
		
		public:
			DBusDevice(Device *dev, QDBusConnection *connection, const QString &path);
			virtual ~DBusDevice();
			
			QString getPath();
			void registerAll();
			void unregisterAll();
			void setAllDBusConnection(QDBusConnection * connection);

		public slots:
			libnutcommon::DeviceProperties getProperties();
			QList<QDBusObjectPath> getEnvironments();
			libnutcommon::DeviceConfig getConfig();
			Q_NOREPLY void enable();
			Q_NOREPLY void disable();
			Q_NOREPLY void setEnvironment(const QDBusObjectPath &path);
			Q_NOREPLY void setEnvironment(qint32 env) {
				m_device->setUserPreferredEnvironment(env);
			}
			qint32 getState() { return m_device->getState(); }
			qint32 getType() { return m_device->hasWLAN() ? libnutcommon::DT_AIR : libnutcommon::DT_ETH; }
			QString getActiveEnvironment();
			qint32 getActiveEnvironmentIndex() {
				return m_device->getEnvironment();
			}
			QList<qint32> getEnvironmentIds();
			QString getEssid() { return m_device->essid(); }
		
		signals:
			void stateChanged(int newState, int oldState);
			void environmentChangedActive(const QString &objectpath);
			void environmentChangedActive(qint32 envId);
			void newWirelessNetworkFound();
	};
	
	class DBusEnvironment: public QDBusAbstractAdaptor {
		Q_OBJECT
		Q_CLASSINFO("D-Bus Interface", "de.unistuttgart.nut" ".Environment")
		private:
			Environment *m_environment;
			QDBusConnection *m_dbusConnection;
			QList<DBusInterface_IPv4*> m_dbusInterfacesIPv4;
			#ifdef IPv6
			QList<DBusInterface_IPv6*> m_dbusInterfacesIPv6;
			#endif
			QString m_dbusPath;
			libnutcommon::EnvironmentProperties m_dbusProperties;
			Device * m_device;
			bool registered;

		private slots:
			void selectResultReady();
		public:
			DBusEnvironment(Environment *env, QDBusConnection *connection, const QString &path, Device* dev);
			virtual ~DBusEnvironment();
			inline Environment * getEnvironment() const { return m_environment; }
	
			QString getPath();
			void registerAll();
			void unregisterAll();
			void setAllDBusConnection(QDBusConnection * connection);
			void emitChange(bool change);
	
		public slots:
			libnutcommon::EnvironmentProperties getProperties();
			libnutcommon::EnvironmentConfig getConfig();
			QList<QDBusObjectPath> getInterfaces();
			libnutcommon::SelectResult getSelectResult();
			QVector<libnutcommon::SelectResult> getSelectResults();

			qint32 getID() { return m_environment->getID(); }
			QString getName() { return m_environment->getName(); }
			bool getState() { return (m_device->getEnvironment() == m_environment->getID()); }
			QList<qint32> getInterfaceIds();
		signals:
			void interfaceAdded(const QDBusObjectPath &objectpath);
			void interfaceRemoved(const QDBusObjectPath &objectpath);
			void selectsResultChanged(libnutcommon::SelectResult result, QVector<libnutcommon::SelectResult> results);
			void stateChanged(bool state);
	};

	class DBusInterface_IPv4: public QDBusAbstractAdaptor {
		Q_OBJECT
		Q_CLASSINFO("D-Bus Interface", "de.unistuttgart.nut" ".Interface_IPv4")
		private:
			Interface_IPv4 *m_interface;
			QDBusConnection *m_dbusConnection;
			QString m_dbusPath;
			libnutcommon::InterfaceProperties m_dbusProperties;
			bool registered;
		private slots:
			void interfaceStatusChanged(libnutcommon::InterfaceState state);
		public:
			DBusInterface_IPv4(Interface_IPv4 *iface, QDBusConnection *connection, const QString &path);
			virtual ~DBusInterface_IPv4();
	
			QString getPath();
			void registerAll();
			void unregisterAll();
			void setAllDBusConnection(QDBusConnection * connection) { m_dbusConnection = connection; }
		
		public slots:
			libnutcommon::InterfaceProperties getProperties();
			libnutcommon::IPv4Config getConfig();
	
			bool needUserSetup() { return m_interface->needUserSetup(); }
			bool setUserConfig(libnutcommon::IPv4UserConfig userConfig) { return m_interface->setUserConfig(userConfig); }
			libnutcommon::IPv4UserConfig getUserConfig() { return m_interface->getUserConfig(); }

			//generic information
			qint32 getState() { return m_interface->getState(); }
			QHostAddress getIP() { return m_interface->ip; }
			QHostAddress getNetmask() { return m_interface->netmask; }
			QHostAddress getGateway() { return m_interface->gateway; }
			QList<QHostAddress> getDns() { return m_interface->dnsserver; }

			//void activate() { m_interface->start();}
			//void deactivate() { m_interface->stop();}
		
		signals:
			void stateChanged(const libnutcommon::InterfaceProperties &properties);
	};
	#ifdef IPv6
	class DBusInterface_IPv6: public QDBusAbstractAdaptor {
		Q_OBJECT
		Q_CLASSINFO("D-BUS Interface", "de.unistuttgart.nut" ".Interface_IPv6");
		
	}
	#endif
}

#endif
