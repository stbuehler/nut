//
// C++ Implementation: wirelesssettings
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QNUT_NO_WIRELESS
#include <QHeaderView>
#include <QMessageBox>
#include <QMenu>
#include <QListView>
#include <libnutclient/client.h>
#include "wirelesssettings.h"
#include "managedapmodel.h"
#include "common.h"
#include "constants.h"
#include "availableapmodel.h"
#include "accesspointconfig.h"
#include "adhocconfig.h"
#include <libnutwireless/cwireless.h>
#include <libnutwireless/conversion.h>

namespace qnut {
	//TODO make only theese functions available that are supported by the wpa_supplicant
	using namespace libnutcommon;
	using namespace libnutclient;
	using namespace libnutwireless;
	
	CWirelessSettings::CWirelessSettings(CDevice * wireless, QWidget * parent) : QWidget(parent), m_Device(wireless) {
		ui.setupUi(this);
		
		
		QListView * hiddenListView = new QListView(this);
		hiddenListView->hide();
		hiddenListView->setViewMode(QListView::IconMode);
		hiddenListView->setIconSize(QSize(24, 24));
		hiddenListView->setGridSize(QSize(96, 64));
		
		m_VisibleAPView = ui.managedView;
		m_HiddenAPView = hiddenListView;
		
		ui.managedAPLayout->addWidget(m_HiddenAPView);
		
		setWindowTitle(tr("Wireless Settings for \"%1\"").arg(m_Device->getName()));
		setWindowIcon(QIcon(UI_ICON_AIR));
		
		ui.nameLabel->setText(m_Device->getName());
		
		m_ManagedAPProxyModel = new CManagedAPProxyModel(this);
		m_ManagedAPModel = new CManagedAPModel(m_Device->getWireless()->getWpaSupplicant(), this);
		m_ManagedAPProxyModel->setSourceModel(m_ManagedAPModel);
		
		m_AvailableAPProxyModel = new CAvailableAPProxyModel(this);
		m_AvailableAPModel = new CAvailableAPModel(m_Device->getWireless()->getHardware(), this);
		m_AvailableAPProxyModel->setSourceModel(m_AvailableAPModel);
		
		createActions();
		
		m_VisibleAPView->setModel(m_ManagedAPProxyModel);
		m_HiddenAPView->setModel(m_ManagedAPProxyModel);
		ui.availableView->setModel(m_AvailableAPProxyModel);
		
		ui.managedView->header()->setResizeMode(QHeaderView::ResizeToContents);
		ui.availableView->header()->setResizeMode(QHeaderView::ResizeToContents);
		
		updateUi(m_Device->getState());
		
		ui.managedView->header()->setMinimumSectionSize(-1);
		ui.availableView->header()->setMinimumSectionSize(-1);
		
		connect(m_Device, SIGNAL(stateChanged(libnutcommon::DeviceState)), this, SLOT(updateUi(libnutcommon::DeviceState)));
		
		connect(m_VisibleAPView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(handleManagedAPSelectionChanged(const QItemSelection &, const QItemSelection &)));
		connect(m_HiddenAPView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(handleManagedAPSelectionChanged(const QItemSelection &, const QItemSelection &)));
		
		connect(ui.availableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNetwork()));
		connect(m_VisibleAPView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(switchToSelectedNetwork()));
		connect(m_HiddenAPView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(switchToSelectedNetwork()));
		
		connect(ui.availableAPFilterEdit, SIGNAL(textChanged(const QString &)), m_AvailableAPProxyModel, SLOT(setFilterWildcard(const QString &)));
		
		if (m_Device->getWireless()) {
//			connect(m_Device->getWpaSupplicant(), SIGNAL(stateChanged(bool)), this, SLOT(setEnabled(bool)));
			connect(m_SaveNetworksAction, SIGNAL(triggered()), m_Device->getWireless()->getWpaSupplicant(), SLOT(save_config()));
			connect(m_RescanNetworksAction, SIGNAL(triggered()), m_Device->getWireless(), SLOT(scan()));
			connect(m_Device->getWireless()->getHardware(), SIGNAL(signalQualityUpdated(libnutwireless::SignalQuality)),
				this, SLOT(updateSignalInfo(libnutwireless::SignalQuality)));
		}
	}
	
	inline void CWirelessSettings::createActions() {
		QAction * enableNetworksAction;
		
		QAction * addNetworkAction;
		QAction * addAdhocAction;
		
		QAction * reloadNetworksAction;
		
		QMenu * manageNetworksMenu;
		
		m_EnableNetworkAction    = new QAction(QIcon(UI_ICON_ENABLE), tr("&Enable"), this);
		enableNetworksAction     = new QAction(QIcon(UI_ICON_ENABLE_ALL), tr("Enable &all"), this);
		m_DisableNetworkAction   = new QAction(QIcon(UI_ICON_DISABLE), tr("&Disable"), this);
		m_SwitchNetworkAction    = new QAction(QIcon(UI_ICON_FORCE), tr("S&witch"), this);
		m_ConfigureNetworkAction = new QAction(QIcon(UI_ICON_CONFIGURE), tr("&Configure..."), this);
		addNetworkAction         = new QAction(QIcon(UI_ICON_ADD), tr("Add &network"), this);
		addAdhocAction           = new QAction(QIcon(UI_ICON_ADD_ADHOC), tr("Add ad-&hoc"), this);
		reloadNetworksAction     = new QAction(QIcon(UI_ICON_RELOAD), tr("Re&load configuration"), this);
		m_SaveNetworksAction     = new QAction(QIcon(UI_ICON_SAVE), tr("&Save configuration"), this);
		m_AutoSaveNetworksAction = new QAction(/*QIcon(UI_ICON_AUTOSAVE), */tr("&Autosave configuration"), this);
		m_RemoveNetworkAction    = new QAction(QIcon(UI_ICON_REMOVE), tr("&Remove"), this);
		m_ToggleDetailsAction    = new QAction(QIcon(UI_ICON_DETAILED), tr("Detailed &view"), this);
		m_RescanNetworksAction   = new QAction(QIcon(UI_ICON_SEARCH), tr("Scan ne&tworks"), this);
		
		manageNetworksMenu       = new QMenu(tr("More..."));
		manageNetworksMenu->setIcon(QIcon(UI_ICON_EDIT));
		
		m_AutoSaveNetworksAction->setCheckable(true);
		m_AutoSaveNetworksAction->setChecked(false);
		
		m_ToggleDetailsAction->setCheckable(true);
		m_ToggleDetailsAction->setChecked(true);
		
		m_EnableNetworkAction->setEnabled(false);
		m_DisableNetworkAction->setEnabled(false);
		m_SwitchNetworkAction->setEnabled(false);
		m_ConfigureNetworkAction->setEnabled(false);
		m_RemoveNetworkAction->setEnabled(false);
		
		connect(m_EnableNetworkAction, SIGNAL(triggered()), this, SLOT(enableSelectedNetwork()));
		connect(enableNetworksAction, SIGNAL(triggered()), this, SLOT(enableNetworks()));
		connect(m_DisableNetworkAction, SIGNAL(triggered()), this, SLOT(disableSelectedNetwork()));
		connect(m_SwitchNetworkAction, SIGNAL(triggered()), this, SLOT(switchToSelectedNetwork()));
		connect(m_ConfigureNetworkAction, SIGNAL(triggered()), this, SLOT(configureSelectedNetwork()));
		connect(m_RemoveNetworkAction, SIGNAL(triggered()), this, SLOT(removeSelectedNetwork()));
		
		connect(addNetworkAction, SIGNAL(triggered()), this, SLOT(addNetwork()));
		connect(addAdhocAction, SIGNAL(triggered()), this, SLOT(addAdhoc()));
		
		connect(m_ToggleDetailsAction, SIGNAL(toggled(bool)), this, SLOT(toggleDetails(bool)));
		connect(reloadNetworksAction, SIGNAL(triggered()), m_ManagedAPModel, SLOT(updateNetworks()));
		
		manageNetworksMenu->addAction(enableNetworksAction);
		manageNetworksMenu->addSeparator();
		manageNetworksMenu->addAction(m_SaveNetworksAction);
		manageNetworksMenu->addAction(reloadNetworksAction);
		manageNetworksMenu->addSeparator();
		manageNetworksMenu->addAction(m_AutoSaveNetworksAction);
		
		m_VisibleAPView->addAction(m_EnableNetworkAction);
		m_VisibleAPView->addAction(m_DisableNetworkAction);
		m_VisibleAPView->addAction(getSeparator(this));
		m_VisibleAPView->addAction(m_SwitchNetworkAction);
		m_VisibleAPView->addAction(m_ConfigureNetworkAction);
		m_VisibleAPView->addAction(getSeparator(this));
		m_VisibleAPView->addAction(addAdhocAction);
		m_VisibleAPView->addAction(m_RemoveNetworkAction);
		m_VisibleAPView->addAction(getSeparator(this));
		m_VisibleAPView->addAction(manageNetworksMenu->menuAction());
		m_VisibleAPView->setContextMenuPolicy(Qt::ActionsContextMenu);
		m_HiddenAPView->addActions(m_VisibleAPView->actions());
		m_HiddenAPView->setContextMenuPolicy(Qt::ActionsContextMenu);
		
		ui.availableView->addAction(addNetworkAction);
		ui.availableView->addAction(getSeparator(this));
		ui.availableView->addAction(m_RescanNetworksAction);
		ui.availableView->setContextMenuPolicy(Qt::ActionsContextMenu);
		
		ui.enableNetworkButton->setDefaultAction(m_EnableNetworkAction);
		ui.disableNetworkButton->setDefaultAction(m_DisableNetworkAction);
		ui.switchNetworkButton->setDefaultAction(m_SwitchNetworkAction);
		ui.configureNetworkButton->setDefaultAction(m_ConfigureNetworkAction);
		ui.removeNetworkButton->setDefaultAction(m_RemoveNetworkAction);
		ui.toggleDetailsButton->setDefaultAction(m_ToggleDetailsAction);
		
		ui.manageNetworksButton->setMenu(manageNetworksMenu);
		ui.manageNetworksButton->setText(manageNetworksMenu->title());
		ui.manageNetworksButton->setIcon(QIcon(UI_ICON_EDIT));
		
		ui.addNetworkButton->setDefaultAction(addNetworkAction);
		ui.addAdhocButton->setDefaultAction(addAdhocAction);
		ui.rescanNetworksButton->setDefaultAction(m_RescanNetworksAction);
	}
	
	void CWirelessSettings::updateSignalInfo(SignalQuality signal) {
		QString quality = QString::number(signal.quality.value) + '/' + QString::number(signal.quality.maximum);
		QString level;
		QString noise;
		bool showLevelNoise = true;
		
		switch (signal.type) {
		case WSR_RCPI:
			level = QString::number(signal.level.rcpi);
			noise = QString::number(signal.noise.rcpi);
			break;
		case WSR_ABSOLUTE:
			level = QString::number(signal.level.nonrcpi.value);
			noise = QString::number(signal.noise.nonrcpi.value);
			break;
		case WSR_RELATIVE:
			level = QString::number(signal.level.nonrcpi.value) + '/' + QString::number(signal.level.nonrcpi.maximum);
			noise = QString::number(signal.noise.nonrcpi.value) + '/' + QString::number(signal.noise.nonrcpi.maximum);
			break;
		default:
			showLevelNoise = false;
			break;
		}
		
		ui.signalLabel->setText(signal.ssid + " (" +
			tr("Channel") + ' ' + QString::number(frequencyToChannel(signal.frequency)) + ')');
		ui.bssidLabel->setText(signal.bssid.toString());
		ui.qualityLabel->setText(tr("Quality") + ": " + quality);
		ui.rateLabel->setText(tr("Bitrate") + ": " + QString::number(signal.bitrates[0] / 1000000) + "Mb/s");
		
		ui.qualityLevelLine->setVisible(showLevelNoise);
		ui.levelLabel->setVisible(showLevelNoise);
		ui.noiseLabel->setVisible(showLevelNoise);
		if (showLevelNoise) {
			ui.levelLabel->setText(tr("Level") + ": " + level + "dBm");
			ui.noiseLabel->setText(tr("Noise") + ": " + noise + "dBm");
		}
	}
	
	void CWirelessSettings::handleManagedAPSelectionChanged(const QItemSelection & selected, const QItemSelection &) {
		m_EnableNetworkAction->setDisabled(selected.isEmpty());
		m_DisableNetworkAction->setDisabled(selected.isEmpty());
		m_SwitchNetworkAction->setDisabled(selected.isEmpty());
		m_ConfigureNetworkAction->setDisabled(selected.isEmpty());
		m_RemoveNetworkAction->setDisabled(selected.isEmpty());
	}
	
	void CWirelessSettings::updateUi(DeviceState state) {
		ui.managedAvailableAPSplitter->setEnabled(state >= DS_ACTIVATED && m_Device->getWireless());
		ui.actionsWidget->setEnabled(state >= DS_ACTIVATED && m_Device->getWireless());
		
		ui.iconLabel->setPixmap(QPixmap(iconFile(m_Device)));
		ui.stateLabel->setText(toStringTr(state));
		
		if (state <= DS_ACTIVATED) {
			ui.signalLabel->setText(tr("no signal info"));
			ui.bssidLabel->setText("00:00:00:00:00:00");
			ui.qualityLabel->setText(tr("Quality") + ": 0/0");
			ui.rateLabel->setText(tr("Bitrate") + ": 0Mb/s");
			ui.levelLabel->setText(tr("Level") + ": 0");
			ui.noiseLabel->setText(tr("Noise") + ": 0");
		}
	}
	
	QModelIndex CWirelessSettings::selectedIndex(QAbstractItemView * view) {
		QModelIndexList selectedIndexes = view->selectionModel()->selectedIndexes();
		
		if (selectedIndexes.isEmpty())
			return QModelIndex();
		
		QSortFilterProxyModel * proxyModel = qobject_cast<QSortFilterProxyModel *>(view->model());
		if (proxyModel)
			return proxyModel->mapToSource(selectedIndexes[0]);
		else
			return selectedIndexes[0];
	}
	
	void CWirelessSettings::addNetwork() {
		QModelIndex index = selectedIndex(ui.availableView);
		
		bool accepted = false;
		if (index.isValid()) {
			ScanResult scan = m_AvailableAPModel->cachedScans()[index.internalId()];
			if (scan.opmode == OPM_ADHOC) {
				CAdhocConfig dialog(m_Device->getWireless(), this);
				accepted = dialog.execute(scan);
			}
			else {
				CAccessPointConfig dialog(m_Device->getWireless()->getWpaSupplicant(), this);
				accepted = dialog.execute(scan);
			}
		}
		else {
			CAccessPointConfig dialog(m_Device->getWireless()->getWpaSupplicant(), this);
			accepted = dialog.execute();
		}
		if (accepted && m_AutoSaveNetworksAction->isChecked())
			m_SaveNetworksAction->trigger();
	}
	
	void CWirelessSettings::addAdhoc() {
		CAdhocConfig dialog(m_Device->getWireless(), this);
		if (dialog.execute() && m_AutoSaveNetworksAction->isChecked())
			m_SaveNetworksAction->trigger();
	}
	
	void CWirelessSettings::switchToSelectedNetwork() {
		int id = m_ManagedAPModel->cachedNetworks()[selectedIndex(m_VisibleAPView).internalId()].id;
		
		m_Device->getWireless()->getWpaSupplicant()->selectNetwork(id);
		if (m_AutoSaveNetworksAction->isChecked())
			m_SaveNetworksAction->trigger();
	}
	
	void CWirelessSettings::removeSelectedNetwork() {
		ShortNetworkInfo network = m_ManagedAPModel->cachedNetworks()[selectedIndex(m_VisibleAPView).internalId()];
		
		QString message = tr("Are you sure to remove \"%1\"?").arg(network.ssid);
		if (m_AutoSaveNetworksAction->isChecked())
			message += '\n' + tr("This action is not reversible.");
		
		if (QMessageBox::question(this, tr("Removing a managed network"), message,
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
			m_Device->getWireless()->getWpaSupplicant()->removeNetwork(network.id);
			if (m_AutoSaveNetworksAction->isChecked())
				m_SaveNetworksAction->trigger();
	}
	
	void CWirelessSettings::configureSelectedNetwork() {
		ShortNetworkInfo network = m_ManagedAPModel->cachedNetworks()[selectedIndex(m_VisibleAPView).internalId()];
		
		bool accepted = false;
		if (network.adhoc) {
			CAdhocConfig dialog(m_Device->getWireless(), this);
			accepted = dialog.execute(network.id);
		}
		else {
			CAccessPointConfig dialog(m_Device->getWireless()->getWpaSupplicant(), this);
			accepted = dialog.execute(network.id);
		}
		
		if (accepted && m_AutoSaveNetworksAction->isChecked())
			m_SaveNetworksAction->trigger();
	}
	
	void CWirelessSettings::enableSelectedNetwork() {
		int id = m_ManagedAPModel->cachedNetworks()[selectedIndex(m_VisibleAPView).internalId()].id;
		
		m_Device->getWireless()->getWpaSupplicant()->enableNetwork(id);
		
		if (m_AutoSaveNetworksAction->isChecked())
			m_SaveNetworksAction->trigger();
	}
	
	void CWirelessSettings::disableSelectedNetwork() {
		int id = m_ManagedAPModel->cachedNetworks()[selectedIndex(m_VisibleAPView).internalId()].id;
		
		m_Device->getWireless()->getWpaSupplicant()->disableNetwork(id);
		
		if (m_AutoSaveNetworksAction->isChecked())
			m_SaveNetworksAction->trigger();
	}
	
	void CWirelessSettings::enableNetworks() {
		QList<ShortNetworkInfo> networks = m_ManagedAPModel->cachedNetworks();
		foreach (ShortNetworkInfo i, networks) {
			m_Device->getWireless()->getWpaSupplicant()->enableNetwork(i.id);
		}
		
		if (m_AutoSaveNetworksAction->isChecked())
			m_SaveNetworksAction->trigger();
	}
	
	void CWirelessSettings::toggleDetails(bool value) {
		QAbstractItemView * buffer = m_VisibleAPView;
		m_VisibleAPView = m_HiddenAPView;
		m_HiddenAPView = buffer;
		
		m_HiddenAPView->hide();
// 		ui.managedWidget->layout()->removeWidget(m_HiddenAPView);
// 		ui.managedWidget->layout()->addWidget(m_VisibleAPView);
		m_VisibleAPView->selectionModel()->clear();
		m_VisibleAPView->show();
		
		if (value) {
			
/*			ui.managedView->showColumn(UI_MANAP_ID);
			ui.managedView->showColumn(UI_MANAP_BSSID);*/
			ui.availableView->showColumn(UI_AVLAP_BSSID);
			ui.availableView->showColumn(UI_AVLAP_CHANNEL);
			ui.availableView->showColumn(UI_AVLAP_LEVEL);
		}
		else {
/*			ui.managedView->hideColumn(UI_MANAP_ID);
			ui.managedView->hideColumn(UI_MANAP_BSSID);*/
			ui.availableView->hideColumn(UI_AVLAP_BSSID);
			ui.availableView->hideColumn(UI_AVLAP_CHANNEL);
			ui.availableView->hideColumn(UI_AVLAP_LEVEL);
		}
/*		ui.managedView->header()->resizeSections(QHeaderView::ResizeToContents);
		ui.availableView->header()->resizeSections(QHeaderView::ResizeToContents);*/
	}
}
#endif
