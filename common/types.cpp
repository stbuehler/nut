#include "types.h"
#include "config.h"

namespace libnut {
	QDBusArgument &operator<< (QDBusArgument &argument, const DeviceProperties & devprop) {
		argument.beginStructure();
		argument << devprop.name << devprop.activeEnvironment << (int) devprop.state << (int) devprop.type;
		argument.endStructure();
		return argument;
	}
	const QDBusArgument &operator>> (const QDBusArgument &argument, DeviceProperties &devprop) {
		int tmp;
		argument.beginStructure();
		argument >> devprop.name >> devprop.activeEnvironment >> tmp;
		devprop.state = (DeviceState) tmp;
		argument >> tmp;
		devprop.type = (DeviceType) tmp;
		argument.endStructure();
		return argument;
	}
	
	QDBusArgument &operator<< (QDBusArgument &argument, const WlanScanresult &scanres) {
		argument.beginStructure();
		argument << scanres.essid << scanres.channel << scanres.bssid << scanres.flags << scanres.signallevel << (int) scanres.encryption;
		argument.endStructure();
		return argument;
	}
	const QDBusArgument &operator>> (const QDBusArgument &argument, WlanScanresult &scanres) {
		int tmp;
		argument.beginStructure();
		argument >> scanres.essid >> scanres.channel >> scanres.bssid >> scanres.flags >> scanres.signallevel >> tmp;
		scanres.encryption = (WlanEncryptionType) tmp;
		argument.endStructure();
		return argument;
	}
	
	QDBusArgument &operator<< (QDBusArgument &argument, const WlanNetworkProperties &wlanprop) {
		argument.beginStructure();
		argument << wlanprop.scanresult << wlanprop.password << wlanprop.proto << wlanprop.key_mgmt;
		argument.endStructure();
		return argument;
	}
	const QDBusArgument &operator>> (const QDBusArgument &argument, WlanNetworkProperties &wlanprop) {
		argument.beginStructure();
		argument >> wlanprop.scanresult >> wlanprop.password >> wlanprop.proto >> wlanprop.key_mgmt;
		argument.endStructure();
		return argument;
	}
	
	QDBusArgument &operator<< (QDBusArgument &argument, const EnvironmentProperties &envprop) {
		argument.beginStructure();
		argument << envprop.name;
		argument.endStructure();
		return argument;
	};
	const QDBusArgument &operator>> (const QDBusArgument &argument, EnvironmentProperties &envprop) {
		argument.beginStructure();
		argument >> envprop.name;
		return argument;
	}
	QDBusArgument &operator<< (QDBusArgument &argument, const InterfaceProperties &ifprop) {
		argument.beginStructure();
		argument << ifprop.active << ifprop.userDefineable << ifprop.isStatic;
		argument << ifprop.ip.toString() << ifprop.netmask.toString() << ifprop.gateway.toString() << ifprop.dns.toString();
		argument.endStructure();
		return argument;
	}
	const QDBusArgument &operator>> (const QDBusArgument &argument, InterfaceProperties &ifprop) {
		argument.beginStructure();
		QString ip;
		argument >> ifprop.active >> ifprop.userDefineable >> ifprop.isStatic;
		argument >> ip;
		ifprop.ip = QHostAddress(ip);
		argument >> ip;
		ifprop.netmask = QHostAddress(ip);
		argument >> ip;
		ifprop.gateway = QHostAddress(ip);
		argument >> ip;
		ifprop.dns = QHostAddress(ip);
		argument.endStructure();
		return argument;
	}
	
	static void init() {
		qRegisterMetaType<DeviceProperties>("DeviceProperties");
		qRegisterMetaType<EnvironmentProperties>("EnvironmentProperties");
		qRegisterMetaType<InterfaceProperties>("InterfaceProperties");
		qRegisterMetaType<WlanScanresult>("WlanScanresult");
		qRegisterMetaType<QList<libnut::WlanScanresult> >("WlanScanresultList");
		qRegisterMetaType<WlanNetworkProperties>("WlanNetworkProperties");
		
		qDBusRegisterMetaType<DeviceProperties>();
		qDBusRegisterMetaType<EnvironmentProperties>();
		qDBusRegisterMetaType<InterfaceProperties>();
		qDBusRegisterMetaType<WlanScanresult>();
		qDBusRegisterMetaType<QList<libnut::WlanScanresult> >();
		qDBusRegisterMetaType<WlanNetworkProperties>();
	}
}

namespace nut {
	static void init() {
		qRegisterMetaType<Config>("nut::Config");
		qRegisterMetaType<DeviceConfig>("nut::DeviceConfig");
		qRegisterMetaType<SelectRule>("nut::SelectRule");
		qRegisterMetaType<SelectConfig>("nut::SelectConfig");
		qRegisterMetaType<EnvironmentConfig>("nut::EnvironmentConfig");
		qRegisterMetaType<IPv4Config>("nut::IPv4Config");
		qRegisterMetaType< QVector< size_t > >("QVector< size_t >");
		qRegisterMetaType< QVector< QVector< size_t > > >("QVector< QVector< size_t > >");
	
		qDBusRegisterMetaType<Config>();
		qDBusRegisterMetaType<DeviceConfig>();
		qDBusRegisterMetaType<SelectRule>();
		qDBusRegisterMetaType<SelectConfig>();
		qDBusRegisterMetaType<EnvironmentConfig>();
		qDBusRegisterMetaType<IPv4Config>();
		qDBusRegisterMetaType< QVector< size_t > >();
		qDBusRegisterMetaType< QVector< QVector< size_t > > >();
	}
}

namespace common {
	void init() {
		static int done = 0;
		if (done) return;
		done = 1;
		
		qRegisterMetaType<QHostAddress>("QHostAddress");
		qRegisterMetaType<QList<QHostAddress> >("QList<QHostAddress>");
		qDBusRegisterMetaType<QHostAddress>();
		qDBusRegisterMetaType<QList<QHostAddress> >();
		
		libnut::init();
		nut::init();
	}
}
