//
// C++ Implementation: adhocconfig
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QNUT_NO_WIRELESS
#include <QMessageBox>
#include <libnutwireless/cwireless.h>
#include <libnutwireless/conversion.h>
#include "adhocconfig.h"

#define FLAG_PREPARE_OUTPUT(a, b, c) if(a & c) b << #c;

namespace qnut {
	using namespace libnutwireless;

	CAdhocConfig::CAdhocConfig(CWireless * wireless, QWidget * parent) : QDialog(parent) {
		m_Supplicant = wireless->getWpaSupplicant();
		m_WirelessHW = wireless->getHardware();
		
		ui.setupUi(this);
		
		foreach (quint8 i, m_WirelessHW->getSupportedChannels()) {
			ui.channelCombo->addItem(QString::number(i));
		}
		
		QRegExp regexp("[0123456789abcdefABCDEF]*");
		m_HexValidator = new QRegExpValidator(regexp, this);
		
		connect(ui.ssidHexCheck, SIGNAL(toggled(bool)), this, SLOT(convertSSID(bool)));
		connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(verifyConfiguration()));
	}
	
	CAdhocConfig::~CAdhocConfig() {
		delete m_HexValidator;
	}
	
	void CAdhocConfig::verifyConfiguration() {
		NetconfigStatus status;
		CNetworkConfig config;
		
		if (!ui.ssidEdit->text().isEmpty())
			config.set_ssid( ui.ssidHexCheck->isChecked() ? ui.ssidEdit->text() : '\"' + ui.ssidEdit->text() + '\"');
		config.set_frequency(channelToFrequency(ui.channelCombo->currentText().toInt()));
		config.set_mode(QOOL_TRUE);
		
		switch (ui.encCombo->currentIndex()) {
		case 0:
			config.set_key_mgmt(KM_OFF);
			break;
		case 1:
			config.set_wep_tx_keyidx(0);
			config.set_key_mgmt(KM_NONE);
			
			if (!(ui.pskLeaveButton->isChecked() || ui.pskEdit->text().isEmpty()))
				config.set_wep_key0('\"' + ui.pskEdit->text() + '\"');
			break;
		default:
			if (ui.encCombo->currentText() == "CCMP")
				config.set_group(GCI_CCMP);
			else if (ui.encCombo->currentText() == "TKIP")
				config.set_group(GCI_TKIP);
			
			config.set_proto(PROTO_WPA);
			config.set_pairwise(PCI_NONE);
			config.set_key_mgmt(KM_WPA_NONE);
			
			if (!(ui.pskLeaveButton->isChecked() || (ui.pskEdit->text().isEmpty())))
				config.set_psk('\"' + ui.pskEdit->text() + '\"');
			break;
		}
		
		if (m_CurrentID > -1) {
			status = m_Supplicant->editNetwork(m_CurrentID, config);
		}
		else {
			status = m_Supplicant->addNetwork(config);
		}
		
		QStringList errormsg;
		
		if (status.failures != NCF_NONE) {
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_ALL)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_SSID)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_BSSID)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_DISABLED)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_ID_STR)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_SCAN_SSID)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_PRIORITY)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_MODE)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_FREQ)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_PROTO)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_KEYMGMT)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_AUTH_ALG)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_PAIRWISE)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_GROUP)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_PSK)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_EAPOL_FLAGS)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_MIXED_CELL)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_PROA_KEY_CACHING)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_WEP_KEY0)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_WEP_KEY1)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_WEP_KEY2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_WEP_KEY3)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_WEP_KEY_IDX)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, NCF_PEERKEY)
		}
		
		if (status.eap_failures != ENCF_NONE) {
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_ALL)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_EAP)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_IDENTITY)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_ANON_IDENTITY)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_PASSWD)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_CA_CERT)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_CA_PATH)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_CLIENT_CERT)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_PRIVATE_KEY)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_PRIVATE_KEY_PASSWD)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_DH_FILE)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_SUBJECT_MATCH)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_ALTSUBJECT_MATCH)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_PHASE1)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_PHASE2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_CA_CERT2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_CA_PATH2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_CLIENT_CERT2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_PRIVATE_KEY2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_PRIVATE_KEY2_PASSWD)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_DH_FILE2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_SUBJECT_MATCH2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_ALTSUBJECT_MATCH2)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_FRAGMENT_SIZE)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_EAPPSK)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_NAI)
			FLAG_PREPARE_OUTPUT(status.failures, errormsg, ENCF_PAC_FILE)
		}
		
		if (!errormsg.isEmpty()) {
			QString errors = errormsg.join(", ");
			QMessageBox::critical(this, tr("Error on applying settings"),
				tr("WPA supplicant reported the following errors:") + '\n' + errors);
			qDebug(errors.toAscii().data());
			return;
		}
		
		accept();
	}
	
	bool CAdhocConfig::execute() {
		ui.pskLeaveButton->setVisible(false);
		ui.pskLeaveButton->setChecked(false);
		
		m_CurrentID = -1;
		return exec();
	}
	
	bool CAdhocConfig::execute(int id) {
		CNetworkConfig config = m_Supplicant->getNetworkConfig(id);
		
		if (config.get_ssid()[0] == '\"') {
			ui.ssidHexCheck->setChecked(false);
			ui.ssidEdit->setText(config.get_ssid().mid(1, config.get_ssid().length()-2));
		}
		else {
			ui.ssidHexCheck->setChecked(true);
			ui.ssidEdit->setText(config.get_ssid());
		}
		
		if (config.get_key_mgmt() & KM_NONE)
			ui.encCombo->setCurrentIndex(1);
		else if (config.get_key_mgmt() & KM_OFF)
			ui.encCombo->setCurrentIndex(0);
		else if (config.get_key_mgmt() & KM_WPA_NONE) {
			if (config.get_group() & GCI_CCMP)
				ui.encCombo->setCurrentIndex(3);
			else if (config.get_group() & GCI_TKIP)
				ui.encCombo->setCurrentIndex(2);
			else {
				QMessageBox::critical(this, tr("Error reading ap config"),
					tr("Unsupported group cipers retrieved: %1").arg((quint32)(config.get_group())));
				return 0;
			}
		}
		else {
			QMessageBox::critical(this, tr("Error reading ap config"),
				tr("Unsupported key management retrieved: %1").arg((quint32)(config.get_key_mgmt())));
			return 0;
		}
		
		int channel = frequencyToChannel(config.get_frequency());
		int channelIndex = m_WirelessHW->getSupportedChannels().indexOf(channel);
		ui.channelCombo->setCurrentIndex(channelIndex);
		
		ui.pskLeaveButton->setVisible(true);
		ui.pskLeaveButton->setChecked(true);
		
		m_CurrentID = id;
		
		return exec();
	}
	
	bool CAdhocConfig::execute(ScanResult scanResult) {
		ui.ssidEdit->setText(scanResult.ssid);
		
		if (scanResult.keyManagement & KM_NONE)
			ui.encCombo->setCurrentIndex(1);
		else if (scanResult.keyManagement & KM_OFF)
			ui.encCombo->setCurrentIndex(0);
		else if (scanResult.keyManagement & KM_WPA_NONE) {
			if (scanResult.group & GCI_CCMP)
				ui.encCombo->setCurrentIndex(3);
			else if (scanResult.group & GCI_TKIP)
				ui.encCombo->setCurrentIndex(2);
			else {
				QMessageBox::critical(this, tr("Error reading ap config"),
					tr("Unsupported group cipers retrieved: %1").arg(toString(scanResult.group)));
				return 0;
			}
		}
		else {
			QMessageBox::critical(this, tr("Error reading ap config"),
				tr("Unsupported key management retrieved: %1").arg(toString(scanResult.keyManagement)));
			return 0;
		}
		
		int channel = frequencyToChannel(scanResult.freq);
		int channelIndex = m_WirelessHW->getSupportedChannels().indexOf(channel);
		ui.channelCombo->setCurrentIndex(channelIndex);
		
		ui.pskLeaveButton->setVisible(false);
		ui.pskLeaveButton->setChecked(false);
		
		m_CurrentID = -1;
		
		return exec();
	}
	
	void CAdhocConfig::convertSSID(bool hex) {
		if (hex) {
			ui.ssidEdit->setText(ui.ssidEdit->text().toAscii().toHex());
			ui.ssidEdit->setValidator(m_HexValidator);
		}
		else {
			ui.ssidEdit->setText(QByteArray::fromHex(ui.ssidEdit->text().toAscii()));
			ui.ssidEdit->setValidator(0);
		}
	}
}
#endif
