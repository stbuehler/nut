//
// C++ Interface: accesspointconfig
//
// Description: 
//
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QNUT_ACCESSPOINTCONFIG_H
#define QNUT_ACCESSPOINTCONFIG_H

#include <QDialog>
#include <libnut/libnut_cli.h>
#include "ui/ui_apconf.h"

namespace qnut {
	class CAccessPointConfig : public QDialog {
		Q_OBJECT
	private:
		Ui::apconf ui;
		
		QRegExpValidator * hexValidator;
		
		libnutws::CWpa_Supplicant * supplicant;
		
		int currentID;
		
		inline void convertLineEditText(QLineEdit * lineEdit, bool hex);
		inline QString convertQuoted(QString text);
		inline void writeEAPConfig(libnutws::wps_eap_network_config &eap_config);
		inline void readEAPConfig(libnutws::wps_eap_network_config &eap_config);
	public:
		bool execute(libnutws::wps_scan scanResult);
		bool execute(int id);
		
		CAccessPointConfig(libnutws::CWpa_Supplicant * wpa_supplicant, QWidget * parent = 0);
		~CAccessPointConfig();
	private slots:
		void setAuthConfig(int type);
		void setEncConfig(QString text);
		void verifyConfiguration();
		void countPskChars(QString psk);
		void togglePlainPSK(bool show);
		void convertSSID(bool hex);
		void convertWEPKey0(bool hex);
		void convertWEPKey1(bool hex);
		void convertWEPKey2(bool hex);
		void convertWEPKey3(bool hex);
		
		void selectCAFile();
		void selectClientFile();
		void selectKeyFile();
	};
}

#endif
