//
// C++ Implementation: deviceoptions
//
// Description: 
//
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QHeaderView>
#include <QInputDialog>
#include <QProcess>
#include "deviceoptions.h"
#include "common.h"
#include "environmenttreemodel.h"
#include "interfacedetailsmodel.h"
#include "environmentdetailsmodel.h"
#include "ipconfiguration.h"
#include "scriptsettings.h"
#include "wirelesssettings.h"
#include "ipconfiguration.h"
#include <QDebug>

namespace qnut {
	CDeviceOptions::CDeviceOptions(CDevice * parentDevice, QWidget * parent) :
		QWidget(parent),
		settings(UI_PATH_DEV(parentDevice->name) + "dev.conf", QSettings::IniFormat, this)
	{
		device = parentDevice;
		
		if (device->type == DT_AIR)
			wirelessSettings = new CWirelessSettings(device);
		else
			wirelessSettings = NULL;
		
		createView();
		createActions();
		
		trayIcon->setToolTip(shortSummary(device));
		trayIcon->setContextMenu(deviceMenu);
		connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(showTheeseOptions()));
		connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(handleTrayActivated(QSystemTrayIcon::ActivationReason)));
		
		readSettings();
		
		connect(device, SIGNAL(stateChanged(DeviceState)), this, SLOT(handleDeviceStateChange(DeviceState)));
		
		if (device->state == DS_UP)
			ui.environmentTree->expand(ui.environmentTree->model()->index(device->environments.indexOf(device->activeEnvironment), 0));
	}
	
	CDeviceOptions::~CDeviceOptions() {
		disconnect(device, SIGNAL(stateChanged(DeviceState)), this, SLOT(handleDeviceStateChange(DeviceState)));
		writeSettings();
		if (wirelessSettings) {
			wirelessSettings->close();
			delete wirelessSettings;
		}
		delete deviceMenu;
	}
	
	inline void CDeviceOptions::readSettings() {
		settings.beginGroup("Main");
		scriptFlags = settings.value("scriptFlags", 0).toInt();
		trayIcon->setVisible(settings.value("showTrayIcon", false).toBool());
		ui.detailsButton->setChecked(settings.value("showDetails", false).toBool());
		settings.endGroup();
		ui.showTrayCheck->setChecked(trayIcon->isVisible());
		
		if (wirelessSettings) {
			settings.beginGroup("WirelessSettings");
			wirelessSettings->resize(settings.value("size", QSize(646, 322)).toSize());
			wirelessSettings->move(settings.value("pos", QPoint(200, 200)).toPoint());
			settings.endGroup();
		}
	}
	
	inline void CDeviceOptions::writeSettings() {
		settings.beginGroup("Main");
		settings.setValue("scriptFlags", scriptFlags);
		settings.setValue("showTrayIcon", trayIcon->isVisible());
		settings.setValue("showDetails", ui.detailsButton->isChecked());
		settings.endGroup();
		
		if (wirelessSettings) {
			settings.beginGroup("WirelessSettings");
			settings.setValue("size", wirelessSettings->size());
			settings.setValue("pos", wirelessSettings->pos());
			settings.endGroup();
		}
	}
	
	inline void CDeviceOptions::createActions() {
		deviceMenu = new QMenu(device->name, NULL);
		enableDeviceAction = deviceMenu->addAction(QIcon(UI_ICON_DEVICE_ENABLE), tr("Enable device"), device, SLOT(enable()));
		disableDeviceAction = deviceMenu->addAction(QIcon(UI_ICON_DEVICE_DISABLE), tr("Disable device"), device, SLOT(disable()));
		deviceMenu->addSeparator();
		showAction = deviceMenu->addAction(QIcon(UI_ICON_ENVIRONMENT), tr("Environments..."), this, SLOT(showTheeseOptions()));
		deviceSettingsAction = deviceMenu->addAction(QIcon(UI_ICON_SCRIPT_SETTINGS), tr("Scripting settings..."), this, SLOT(openDeviceSettings()));
		deviceMenu->addSeparator();
		wirelessSettingsAction = deviceMenu->addAction(QIcon(UI_ICON_AIR_SETTINGS), tr("Wireless settings..."), this, SLOT(openWirelessSettings()));
		
		enterEnvironmentAction = new QAction(QIcon(UI_ICON_ENVIRONMENT_ENTER), tr("Enter environment"), this);
		ipConfigurationAction  = new QAction(QIcon(UI_ICON_EDIT), tr("Set IP configuration..."), this);
		ui.environmentTree->addAction(enterEnvironmentAction);
		ui.environmentTree->addAction(ipConfigurationAction);
		ui.environmentTree->setContextMenuPolicy(Qt::ActionsContextMenu);

		enableDeviceAction->setEnabled(device->state == DS_DEACTIVATED);
		disableDeviceAction->setDisabled(device->state == DS_DEACTIVATED);
		ipConfigurationAction->setEnabled(false);
		enterEnvironmentAction->setEnabled(false);
		wirelessSettingsAction->setEnabled(device->type == DT_AIR);
		
		connect(ipConfigurationAction, SIGNAL(triggered()), this, SLOT(openIPConfiguration()));
	}
	
	inline void CDeviceOptions::createView() {
		trayIcon = new QSystemTrayIcon(QIcon(iconFile(device)), this);
		
		ui.setupUi(this);
		
		connect(ui.showTrayCheck, SIGNAL(toggled(bool)), trayIcon, SLOT(setVisible(bool)));
		
		ui.environmentTree->setModel(new CEnvironmentTreeModel(device));
		ui.environmentTree->header()->setResizeMode(QHeaderView::ResizeToContents);
		
		connect(ui.environmentTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(handleSelectionChanged(const QItemSelection &, const QItemSelection &)));
		setHeadInfo();
	}
	
	inline void CDeviceOptions::setHeadInfo() {
		ui.iconLabel->setPixmap(QPixmap(iconFile(device)));
		ui.statusLabel->setText(toString(device->state));
	}
	
	void CDeviceOptions::handleTrayActivated(QSystemTrayIcon::ActivationReason reason) {
		if (reason == QSystemTrayIcon::Trigger) {
			showAction->trigger();
		}
	}
	
	void CDeviceOptions::showTheeseOptions() {
		emit showOptionsRequested(this);
	}
	
	void CDeviceOptions::handleSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
		QModelIndexList deselectedIndexes = deselected.indexes();
		QModelIndexList selectedIndexes = selected.indexes();
		
		if (!deselectedIndexes.isEmpty()) {
			QModelIndex targetIndex = deselectedIndexes[0];
			if (!targetIndex.parent().isValid()) {
				CEnvironment * target = (CEnvironment *)(targetIndex.internalPointer());
				disconnect(target, SIGNAL(activeChanged(bool)), enterEnvironmentAction, SLOT(setDisabled(bool)));
				disconnect(enterEnvironmentAction, SIGNAL(triggered()), target, SLOT(enter()));
			}
			
		}
		
		QItemSelectionModel * oldSelectionModel = ui.detailsView->selectionModel();
		QAbstractItemModel * oldItemModel = ui.detailsView->model();
		
		if (!selectedIndexes.isEmpty()) {
			QModelIndex targetIndex = selectedIndexes[0];
			if (!targetIndex.parent().isValid()) {
				CEnvironment * target = static_cast<CEnvironment *>(targetIndex.internalPointer());
				connect(target, SIGNAL(activeChanged(bool)), enterEnvironmentAction, SLOT(setDisabled(bool)));
				connect(enterEnvironmentAction, SIGNAL(triggered()), target, SLOT(enter()));
				
				ipConfigurationAction->setEnabled(false);
				enterEnvironmentAction->setDisabled(target == device->activeEnvironment);
				ui.detailsView->setRootIsDecorated(true);
				ui.detailsView->setModel(new CEnvironmentDetailsModel(target));
				ui.detailsView->expandAll();
			}
			else {
				CInterface * target = static_cast<CInterface *>(targetIndex.internalPointer());
				ipConfigurationAction->setEnabled(target->getConfig().getFlags() &  nut::IPv4Config::DO_USERSTATIC);
				enterEnvironmentAction->setEnabled(false);
				ui.detailsView->setRootIsDecorated(false);
				ui.detailsView->setModel(new CInterfaceDetailsModel(target));
			}
		}
		else {
			ipConfigurationAction->setEnabled(false);
			enterEnvironmentAction->setEnabled(false);
			//workarround für leeres model
			ui.detailsView->setModel(new CInterfaceDetailsModel(NULL));
		}
		
		delete oldSelectionModel;
		delete oldItemModel;
	}
	
	void CDeviceOptions::openIPConfiguration() {
		CIPConfiguration dialog(this);
		QModelIndex selectedIndex = (ui.environmentTree->selectionModel()->selection().indexes())[0];
		
		CInterface * interface = static_cast<CInterface *>(selectedIndex.internalPointer());
		nut::IPv4UserConfig config = interface->getUserConfig(true);
		if (dialog.execute(config)) {
			if (!interface->setUserConfig(config)) {
				qDebug() << "setting user config failed:";
				qDebug() << "ip: " << toStringDefault(config.ip());
				qDebug() << "netmask: " << toStringDefault(config.netmask());
				qDebug() << "gateway: " << toStringDefault(config.gateway());
			}
		}
	}
	
	void CDeviceOptions::openDeviceSettings() {
		CScriptSettings dialog(this);
		dialog.execute(this);
	}
	
	void CDeviceOptions::openWirelessSettings() {
		if (wirelessSettings) {
			wirelessSettings->show();
			wirelessSettings->activateWindow();
		}
	}
	
	void CDeviceOptions::handleDeviceStateChange(DeviceState state) {
		setHeadInfo();
		ui.environmentTree->collapseAll();
		if (state >= DS_UNCONFIGURED)
			ui.environmentTree->expand(ui.environmentTree->model()->index(device->environments.indexOf(device->activeEnvironment), 0));
		
		enableDeviceAction->setEnabled(state == DS_DEACTIVATED);
		disableDeviceAction->setDisabled(state == DS_DEACTIVATED);
		ipConfigurationAction->setEnabled(state == DS_UNCONFIGURED);

		if (!ui.environmentTree->selectionModel()->selectedIndexes().isEmpty()) {
			QModelIndex targetIndex = ui.environmentTree->selectionModel()->selectedIndexes()[0];
			if (!targetIndex.parent().isValid()) {
				CEnvironment * target = (CEnvironment *)(targetIndex.internalPointer());
				enterEnvironmentAction->setDisabled(target == device->activeEnvironment);
			}
			else {
				enterEnvironmentAction->setEnabled(false);
			}
		}
		else {
			enterEnvironmentAction->setEnabled(false);
		}
		if (trayIcon->isVisible()) {
			trayIcon->setToolTip(shortSummary(device));
			trayIcon->setIcon(QIcon(iconFile(device)));
			
			switch (state) {
			case DS_UP:
				emit showMessageRequested(tr("QNUT - %1...").arg(device->name),
					tr("...is now up and running."), trayIcon);
				break;
			case DS_UNCONFIGURED:
				emit showMessageRequested(tr("QNUT - %1...").arg(device->name),
					tr("...got carrier but needs configuration.\n\nKlick here to open the configuration dialog."), trayIcon);
				break;
			case DS_ACTIVATED:
				emit showMessageRequested(tr("QNUT - %1...").arg(device->name),
					tr("...is now activated an waits for carrier."), trayIcon);
				break;
			case DS_DEACTIVATED: 
				emit showMessageRequested(tr("QNUT - %1...").arg(device->name),
					tr("...is now deactivated"), trayIcon);
				break;
			default:
				break;
			}
		}
		else {
			switch (state) {
			case DS_UP:
				emit showMessageRequested(tr("QNUT"),
					tr("%1 is now up and running.").arg(device->name));
				break;
			case DS_UNCONFIGURED:
				emit showMessageRequested(tr("QNUT"),
					tr("%1 got carrier but needs configuration.\n\nKlick here to open the configuration dialog.").arg(device->name));
				break;
			case DS_ACTIVATED:
				emit showMessageRequested(tr("QNUT"),
					tr("%1 is now activated an waits for carrier.").arg(device->name));
				break;
			case DS_DEACTIVATED:
				emit showMessageRequested(tr("QNUT"),
					tr("%1 is now deactivated").arg(device->name));
				break;
			default:
				break;
			}
		}
		//emit showMessage(tr("QNUT"), tr("%1 changed its state to \"%2\"").arg(device->name, toString(state)), 4000);
		
		if (scriptFlags) {//TODO:scripts testen
			QDir workdir(UI_PATH_DEV(device->name));
			bool doExecuteScripts = false;
			QString targetDir;
			switch (state) {
			case DS_UP:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_UP);
				targetDir = UI_DIR_SCRIPT_UP;
				break;
			case DS_UNCONFIGURED:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_UNCONFIGURED);
				targetDir = UI_DIR_SCRIPT_UNCONFIGURED;
				break;
			case DS_CARRIER:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_CARRIER);
				targetDir = UI_DIR_SCRIPT_CARRIER;
				break;
			case DS_ACTIVATED:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_ACTIVATED);
				targetDir = UI_DIR_SCRIPT_ACTIVATED;
				break;
			case DS_DEACTIVATED:
				doExecuteScripts = (scriptFlags & UI_FLAG_SCRIPT_DEACTIVATED);
				targetDir = UI_DIR_SCRIPT_DEACTIVATED;
				break;
			default:
				break;
			}
			
			if (doExecuteScripts && workdir.exists(targetDir)) {
				QStringList env;
				QProcess process;
				env << "QNUT_DEV_NAME="  + device->name;
				env << "QNUT_DEV_STATE=" + nut::toString(state);
				//activeEnvironment workarround
				if ((state == DS_UP) && (device->activeEnvironment != NULL)) {
					env << "QNUT_ENV_NAME=" + device->activeEnvironment->name;
					env << "QNUT_IF_COUNT=" + QString::number(device->activeEnvironment->interfaces.count());
					int j = 0;
					foreach (CInterface * i, device->activeEnvironment->interfaces) {
						env << QString("QNUT_IF_%1=%2").arg(QString::number(j),i->ip.toString());
						j++;
					}
				}
				process.setEnvironment(env);
				workdir.cd(targetDir);
				foreach(QString i, workdir.entryList()) {
					process.startDetached(workdir.path() + i);
				}
			}
		}
	}
};
