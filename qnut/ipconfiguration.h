//
// C++ Interface: ipconfiguration
//
// Description: 
//
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QNUT_IPCONFIGURATION_H
#define QNUT_IPCONFIGURATION_H

#include <QDialog>
#include <libnut/libnut_cli.h>
#include "ui/ui_ipconf.h"

namespace qnut {
	using namespace libnut;

	class CIPConfiguration : public QDialog {
		Q_OBJECT
	private:
		Ui::ipconf ui;
		QList<QHostAddress> dnsList;
	public:
		bool execute(nut::IPv4UserConfig & config);
		CIPConfiguration(QWidget * parent = 0);
		~CIPConfiguration();
	private slots:
		void addDNS();
		void removeDNS();
		void handleSelectionChanged(const QItemSelection & selected, const QItemSelection &);
	};

};

#endif
