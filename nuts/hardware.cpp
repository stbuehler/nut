//
// C++ Implementation: hardware
//
// Description: 
//
//
// Author: Stefan Bühler <stbuehler@web.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "hardware.h"
#include "exception.h"
#include "log.h"
#include "device.h"

extern "C" {
#include <asm/types.h>
// socket, AF_INET, SOCK_RAW
#include <sys/socket.h>
// IPPROTO_RAW
#include <arpa/inet.h>
// fcntl, F_SETFD, FD_CLOEXEC
#include <fcntl.h>
#include <sys/ioctl.h>

#include <netlink/netlink.h>
#include <netlink/msg.h>
#include <netlink/route/link.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/ethtool.h>
#include <linux/sysctl.h>
#include <linux/wireless.h>
// hardware_ext.c
extern struct nla_policy ifla_policy[IFLA_MAX+1];
extern struct nla_policy ifa_ipv4_policy[IFA_MAX+1];
}

#include <QSocketNotifier>
#include <QFile>

namespace nuts {
	HardwareManager::HardwareManager()
	: netlink_fd(-1), ethtool_fd(-1) {
		if (!init_netlink()) {
			throw NetlinkInitException("HardwareManager: Couldn't init netlink");
		}
		if (!init_ethtool()) {
			free_netlink();
			throw EthtoolInitException("HardwareManager: Couldn't init ethtool");
		}
		fcntl(netlink_fd, F_SETFD, FD_CLOEXEC);
		fcntl(ethtool_fd, F_SETFD, FD_CLOEXEC);
		QSocketNotifier *nln = new QSocketNotifier(netlink_fd, QSocketNotifier::Read, this);
		connect(nln, SIGNAL(activated(int)), this, SLOT(read_netlinkmsgs()));
	}
	
	HardwareManager::~HardwareManager() {
		free_ethtool();
		free_netlink();
	}
	
	bool HardwareManager::controlOn(int ifIndex, bool force) {
		if (ifIndex < 0) return false;
		if (!ifup(ifIndex2Name(ifIndex), force))
			return false;
		if (ifIndex >= ifStates.size())
			ifStates.resize(ifIndex+1);
		ifStates[ifIndex] = ifstate(true);
		return true;
	}
	bool HardwareManager::controlOff(int ifIndex) {
		if (ifIndex < 0) return false;
		if (ifIndex < ifStates.size() && ifStates[ifIndex].active) {
			ifStates[ifIndex].active = false;
			ifdown(ifIndex2Name(ifIndex));
		}
		return true;
	}
	bool HardwareManager::controlOn(const QString &ifName, bool force) {
		int ifIndex = ifName2Index(ifName);
		if (ifIndex < 0) return false;
		if (!ifup(ifName, force))
			return false;
		if (ifIndex >= ifStates.size())
			ifStates.resize(ifIndex+1);
		ifStates[ifIndex] = ifstate(true);
//		log << "activated interface " << ifIndex << endl;
		return true;
	}
	bool HardwareManager::controlOff(const QString &ifName) {
		int ifIndex = ifName2Index(ifName);
		if (ifIndex < 0) return false;
		if (ifIndex < ifStates.size() && ifStates[ifIndex].active) {
			ifStates[ifIndex].active = false;
			ifdown(ifName);
		}
		return true;
	}
	
	bool HardwareManager::init_netlink() {
		nlh = nl_handle_alloc();
		if (!nlh) return false;
		nl_socket_set_peer_port(nlh, 0);
		
		if (nl_connect(nlh, NETLINK_ROUTE) != 0) goto cleanup;
		if (nl_socket_add_membership(nlh, RTNLGRP_LINK) != 0) goto cleanup;
		if (nl_socket_add_membership(nlh, RTNLGRP_IPV4_IFADDR) != 0) goto cleanup;
		
		netlink_fd = nl_socket_get_fd(nlh);
		nlcache = rtnl_link_alloc_cache(nlh);
		return true;
	cleanup:
		free_netlink();
		return false;
	}
	void HardwareManager::free_netlink() {
		nl_cache_free(nlcache);
		nl_close(nlh);
		nl_handle_destroy(nlh);
	}
	bool HardwareManager::init_ethtool() {
		ethtool_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
		//ethtool_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
		if (ethtool_fd >= 0) return true;
/*
		ethtool_fd = socket(PF_INET, SOCK_DGRAM, 0);
		if (ethtool_fd >= 0) return true;
		ethtool_fd = socket(PF_PACKET, SOCK_DGRAM, 0);
		if (ethtool_fd >= 0) return true;
		ethtool_fd = socket(PF_INET6, SOCK_DGRAM, 0);
		if (ethtool_fd >= 0) return true;
*/
		return false;
	}
	void HardwareManager::free_ethtool() {
		close(ethtool_fd);
	}
	
	bool HardwareManager::ifup(const QString &ifname, bool force) {
/*		nl_cache_update(nlh, nlcache);
		struct rtnl_link *request = rtnl_link_alloc();
		rtnl_link_set_flags(request, rtnl_link_str2flags("up"));
		QByteArray buf = ifname.toUtf8();
		struct rtnl_link *old = rtnl_link_get_by_name(nlcache, buf.constData());
		rtnl_link_change(nlh, old, request, 0);
		rtnl_link_put(old);
		rtnl_link_put(request);
*/
		struct ifreq ifr;
		if (!ifreq_init(ifr, ifname)) {
			err << QString("Interface name too long") << endl;
			return false;
		}
		if (ioctl(ethtool_fd, SIOCGIFFLAGS, &ifr) < 0) {
			err << QString("Couldn't get flags for interface '%1'").arg(ifname) << endl;
			return false;
		}
		if (ifr.ifr_flags & IFF_UP) {
			if (!force) return false;
	        // "restart" interface to get carrier event
			ifr.ifr_flags &= ~IFF_UP;
			if (ioctl(ethtool_fd, SIOCSIFFLAGS, &ifr) < 0) {
				err << QString("Couldn't set flags for interface '%1'").arg(ifname) << endl;
				return false;
			}
		}
		ifr.ifr_flags |= IFF_UP;
		if (ioctl(ethtool_fd, SIOCSIFFLAGS, &ifr) < 0) {
			err << QString("Couldn't set flags for interface '%1'").arg(ifname) << endl;
			return false;
		}
		return true;
	}
	bool HardwareManager::ifdown(const QString &ifname) {
		struct ifreq ifr;
		if (!ifreq_init(ifr, ifname)) {
			err << QString("Interface name too long") << endl;
			return false;
		}
		if (ioctl(ethtool_fd, SIOCGIFFLAGS, &ifr) < 0) {
			err << QString("Couldn't get flags for interface '%1'").arg(ifname) << endl;
			return false;
		}
		if (!(ifr.ifr_flags & IFF_UP)) {
			err << QString("Interface '%1' is not up").arg(ifname) << endl;
			return false;
		}
		ifr.ifr_flags &= ~IFF_UP;
		if (ioctl(ethtool_fd, SIOCSIFFLAGS, &ifr) < 0) {
			err << QString("Couldn't set flags for interface '%1'").arg(ifname) << endl;
			return false;
			}
		return true;
	}
	QString HardwareManager::ifIndex2Name(int ifIndex) {
		struct ifreq ifr;
		ifreq_init(ifr);
		ifr.ifr_ifindex = ifIndex;
		if (ioctl(ethtool_fd, SIOCGIFNAME, &ifr) < 0) {
			err << QString("Couldn't get interface name for device %1").arg(ifIndex) << endl;
			return QString();
		}
		return QString::fromUtf8(ifr.ifr_name, qstrnlen(ifr.ifr_name, IFNAMSIZ));
	}
	QList<QString> HardwareManager::get_ifNames() {
		QList<QString> ifNames;
		QFile file("/proc/net/dev");
		if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in(&file);
			QString ifs = in.readLine();
			ifs = in.readLine();
			ifs = in.readLine();
			while (!ifs.isEmpty())  {
				ifs.truncate(ifs.indexOf(":"));
				ifNames.append(ifs.mid(ifs.lastIndexOf(" ")+1,ifs.size()));
				ifs = in.readLine();
			}
		}
		else {
			struct ifreq ifr;
			for (int i=0; i<32;i++) { //using nuts on a pc with more than 32 interfaces is insane
				ifreq_init(ifr);
				ifr.ifr_ifindex = i;
				if (ioctl(ethtool_fd, SIOCGIFNAME, &ifr) < 0) {
					continue;
				}
				ifNames.append(QString::fromUtf8(ifr.ifr_name, qstrnlen(ifr.ifr_name, IFNAMSIZ)));
			}
		}
		return ifNames;
	}

	int HardwareManager::ifName2Index(const QString &ifName) {
		struct ifreq ifr;
		if (!ifreq_init(ifr, ifName)) {
			err << QString("Interface name too long") << endl;
			return -1;
		}
		if (ioctl(ethtool_fd, SIOCGIFINDEX, &ifr) < 0) {
			err << QString("Couldn't get interface index of '%1'").arg(ifName) << endl;
			return -1;
		}
		return ifr.ifr_ifindex;
	}
	
	struct nl_handle *HardwareManager::getNLHandle() {
		return nlh;
	}
	
	libnutcommon::MacAddress HardwareManager::getMacAddress(const QString &ifName) {
		struct ifreq ifr;
		if (!ifreq_init(ifr, ifName)) {
			err << QString("Interface name too long") << endl;
			return libnutcommon::MacAddress();
		}
		if (ioctl(ethtool_fd, SIOCGIFHWADDR, &ifr) < 0) {
			err << QString("Couldn't get hardware address of '%1'").arg(ifName) << endl;
			return libnutcommon::MacAddress();
		}
		return libnutcommon::MacAddress((quint8*) ifr.ifr_hwaddr.sa_data);
	}
	
	bool HardwareManager::ifreq_init(struct ifreq &ifr, const QString &ifname) {
		QByteArray buf = ifname.toUtf8();
		if (buf.size() >= IFNAMSIZ) return false;
		ifreq_init(ifr);
		strncpy (ifr.ifr_name, buf.constData(), buf.size());
		return true;
	}
	void HardwareManager::ifreq_init(struct ifreq &ifr) {
		memset((char*) &ifr, 0, sizeof(ifr));
	}
	
	void HardwareManager::read_netlinkmsgs() {
		struct sockaddr_nl peer;
		unsigned char *msg;
		int n, msgsize;
		struct nlmsghdr *hdr;
		
		msgsize = n = nl_recv(nlh, &peer, &msg, 0);
		for (hdr = (struct nlmsghdr*) msg; nlmsg_ok(hdr, n); hdr = (struct nlmsghdr*) nlmsg_next(hdr, &n)) {
//			log << QString("Message type 0x%1").arg(hdr->nlmsg_type, 0, 16) << endl;
			switch (hdr->nlmsg_type) {
				case RTM_NEWLINK: {
					struct ifinfomsg *ifm = (struct ifinfomsg*) nlmsg_data(hdr);
					struct nlattr *tb[IFLA_MAX+1];
					if (nlmsg_parse(hdr, sizeof(*ifm), tb, IFLA_MAX, ifla_policy) < 0) {
						break;
					}
					int ifindex = ifm->ifi_index;
					if (ifindex >= ifStates.size())
						ifStates.resize(ifindex+1);
					if (!ifStates[ifindex].exists) {
						ifStates[ifindex].exists = true;
						QString ifname = ifIndex2Name(ifindex);
//						log << QString("RTM_NEWLINK(%1/%2)").arg(ifindex).arg(ifname) << endl;
						emit newDevice(ifname, ifindex);
					}
					bool carrier = (ifm->ifi_flags & IFF_LOWER_UP) > 0;
					if (carrier != ifStates[ifindex].carrier) {
						ifStates[ifindex].carrier = carrier;
						if (!isControlled(ifindex))
							break;
						QString ifname = ifIndex2Name(ifindex);
						if (carrier) {
							QString essid;
							getEssid(ifname, essid);
							emit gotCarrier(ifname, ifindex, essid);
						} else
							emit lostCarrier(ifname);
					}
					} break;
				case RTM_DELLINK: {
					struct ifinfomsg *ifm = (struct ifinfomsg*) nlmsg_data(hdr);
					struct nlattr *tb[IFLA_MAX+1];
					if (nlmsg_parse(hdr, sizeof(*ifm), tb, IFLA_MAX, ifla_policy) < 0) {
						break;
					}
					int ifindex = ifm->ifi_index;
					struct nlattr *nla_ifname = tb[IFLA_IFNAME];
					QString ifname;
					if (nla_ifname)
						ifname = QString::fromUtf8((char*) nla_data(nla_ifname), nla_len(nla_ifname)-1);
					if (ifname.isNull()) break;
//					log << QString("RTM_DELLINK(%1/%2)").arg(ifindex).arg(ifname) << endl;
					if (ifindex < ifStates.size()) {
						if (ifStates[ifindex].exists) {
							if (ifStates[ifindex].carrier)
								emit lostCarrier(ifname);
							ifStates[ifindex].exists = false;
							ifStates[ifindex].carrier = false;
							ifStates[ifindex].active = false;
						}
					}
					emit delDevice(ifname);
					} break;
			}
		}
		if (msgsize > 0) free(msg);
	}
	bool HardwareManager::isControlled(int ifIndex) {
		if (ifIndex < 0) return false;
		if (ifIndex >= ifStates.size()) return false;
		return ifStates[ifIndex].active;
	}
	
	static void iwreq_init(struct iwreq &iwr) {
		memset((char*) &iwr, 0, sizeof(iwr));
	}
	
	static bool iwreq_init(struct iwreq &iwr, const QString &ifname) {
		QByteArray buf = ifname.toUtf8();
		if (buf.size() >= IFNAMSIZ) return false;
		iwreq_init(iwr);
		strncpy (iwr.ifr_ifrn.ifrn_name, buf.constData(), buf.size());
		return true;
	}
	
	bool HardwareManager::ifExists(const QString &ifName) {
		struct ifreq ifr;
		if (!ifreq_init(ifr, ifName)) {
			err << QString("Interface name too long") << endl;
			return false;
		}
		if (ioctl(ethtool_fd, SIOCGIFFLAGS, &ifr) < 0) {
			return false;
		}
		int ifIndex = ifName2Index(ifName);
		if (ifIndex >= ifStates.size())
			ifStates.resize(ifIndex+1);
		ifStates[ifIndex] = ifstate(false);
		return true;
	}
	
	bool HardwareManager::hasWLAN(const QString &ifName) {
		struct iwreq iwr;
		iwreq_init(iwr, ifName);
		if (ioctl(ethtool_fd, SIOCGIWNAME, &iwr) < 0) return false;
		return true;
	}
	
	bool HardwareManager::getEssid(const QString &ifName, QString &essid) {
		essid = "";
		struct iwreq iwr;
		iwreq_init(iwr, ifName);
		char buf[32];
		memset(buf, 0, sizeof(buf));
		iwr.u.essid.pointer = buf;
		iwr.u.essid.length = sizeof(buf);
		if (ioctl(ethtool_fd, SIOCGIWESSID, &iwr) < 0) return false;
		essid = QString::fromUtf8(buf, qstrnlen(buf, iwr.u.essid.length));
		return true;
	}
}
