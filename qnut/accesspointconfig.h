//
// C++ Interface: accesspointconfig
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QNUT_ACCESSPOINTCONFIG_H
#define QNUT_ACCESSPOINTCONFIG_H

#ifndef QNUT_NO_WIRELESS
#include <QDialog>
#include <libnutwireless/cnetworkconfig.h>
#include <QSignalMapper>
#include "cabstractwifinetconfigdialog.h"
#include "ui_apconfexp.h"

namespace libnutwireless {
	class CWpaSupplicant;
}

namespace qnut {
	/**
	 * @brief CAccessPointConfig provides a dialog to configure a managed nework or add a new network for the given wpa_supplicant.
	 * @author Oliver Groß <z.o.gross@gmx.de>
	 * 
	 * On creation, the CAccessPointConfig sets up the user interface according to the given instance of a wpa_supplicant.
	 * The class provides function to execute the dialog for adding (w/o scan result) and configuring a network (by network id).
	 *
	 * By accepting the settings made in the UI they are verified (on error the dialog stays open).
	 */
	class CAccessPointConfig : public CAbstractWifiNetConfigDialog {
		Q_OBJECT
	public:
		/**
		 * @brief Opens the dialog for adding the given scanned network.
		 * @param scanResult scan result with network configuration to use
		 */
		virtual bool execute(libnutwireless::ScanResult scanResult);
		/**
		 * @brief Opens the dialog for configuring the given managed network
		 * @param id managed network id
		 */
		virtual bool execute(int id);
		/// @brief Opens the dialog for adding a new annonymous network
		virtual bool execute();
		
		static QString lastFileOpenDir() { return m_LastFileOpenDir; }
		static void setLastFileOpenDir(QString value) { m_LastFileOpenDir = value; }
		
		/**
		 * @brief Creates the object and initializes the basic user interface.
		 * @param parent parent widget
		 */
		CAccessPointConfig(libnutwireless::CWpaSupplicant * wpa_supplicant, QWidget * parent = 0);
	protected:
		Ui::apconf ui;
		bool m_WEPEnabled;
		
		libnutwireless::CNetworkConfig m_Config;
		
		struct {
			libnutwireless::GroupCiphers group;
			libnutwireless::PairwiseCiphers pairwise;
			libnutwireless::Protocols protocols;
			libnutwireless::KeyManagement keymgmt;
		} m_OldConfig;
		
		struct FileEditStrings {
			QString title;
			QString filter;
		};
		
		static QString m_LastFileOpenDir;
		
		QButtonGroup * m_EAPPhaseButtons;
		QSignalMapper * m_FileEditMapper;
		QMap<QWidget *, FileEditStrings> m_FileSelectStringMap;
		QMap<QCheckBox *, QLineEdit *> m_HexEditMap;
		
		inline void writeEAPPhaseConfig(libnutwireless::CNetworkConfig & eap_config, int phase);
		inline void writeEAPConfig(libnutwireless::CNetworkConfig & eap_config);
		inline void readEAPPhaseConfig(libnutwireless::CNetworkConfig & eap_config, int phase);
		inline void readEAPConfig(libnutwireless::CNetworkConfig & eap_config);
	protected slots:
		void setAuthConfig(int type);
		void handleRSNModeChanged(int value);
		void setWEPDisabled(bool value);
		void verifyConfiguration();
		void countPskChars(QString psk);
		void togglePlainPSK(bool show);
		
		void convertLineEditText(bool hex);
		void selectFile(QWidget * reciever);
		
		void setUiEAPPhase(int phase);
	};
}
#endif

#endif
