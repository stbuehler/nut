//
// C++ Interface: connectionmanager
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QNUT_CONNECTIONMANAGER_H
#define QNUT_CONNECTIONMANAGER_H

#include <QtGui>
#include <libnutclient/client.h>
#include "ui/ui_connman.h"
#include "trayicon.h"
#include "devicedetails.h"

namespace qnut {
	/**
	 * @brief CConnectionManager acts as the main class (and window) of the application
	 * @author Oliver Groß <z.o.gross@gmx.de>
	 * 
	 * The connection manager itself provides only constructor and destructor.
	 * 
	 * It's the main user interface of QNUT with the following tasks:
	 *  - interact with the libnutclient::CDeviceManager
	 *  - provide an overview of all managed devices and an application log (written to file: "~/.qnut/qnut.log")
	 *  - delegate detailed views and settings of each device, environment and interface
	 */
	class CConnectionManager : public QMainWindow {
		Q_OBJECT
	private:
		Ui::connMan ui;
		
		libnutclient::CDeviceManager m_DeviceManager;
		libnutclient::CLog m_LogFile;
		
		CDeviceDetailsHash m_DeviceDetails;
		QSettings m_Stettings;
		
		CTrayIcon m_TrayIcon;
		QTabWidget m_TabWidget;
		QTreeView m_OverView;
		QTextEdit m_LogEdit;
		
		QAction * m_RefreshDevicesAction;
		QAction * m_ClearLogAction;
		
		QAction * m_EnableDeviceAction;
		QAction * m_DisableDeviceAction;
		
		QAction * m_DeviceSettingsAction;
		QAction * m_WirelessSettingsAction;
		
		inline void createActions();
		inline void distributeActions(int mode = 0);
		
		inline void readSettings();
		inline void writeSettings();
	public:
		/**
		 * @brief Creates the class and initializes the basic user interface and reads settings from "~/.qnut/qnut.conf".
		 * @param parent parent widget
		 */
		CConnectionManager(QWidget * parent = 0);
		
		/// @brief Destroyes the class and writes the settings to "~/.qnut/qnut.conf".
		~CConnectionManager();
	private slots:
		void handleTabChanged(int index);
		void handleSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
		void showLog(bool doShow);
		void updateTrayIconInfo();
		void showMessage(QString title, QString message, QSystemTrayIcon * trayIcon = NULL);
		void addUiDevice(libnutclient::CDevice * device);
		void removeUiDevice(libnutclient::CDevice * device);
		void handleDeviceStateChange(libnutcommon::DeviceState state);
		void showDeviceOptions(QWidget * widget);
		void showAbout();
	};
};

#endif
