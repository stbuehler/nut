//
// C++ Interface: CAdhocConfig
//
// Author: Oliver Groß <z.o.gross@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
#ifndef QNUT_CADHOCCONFIG_H
#define QNUT_CADHOCCONFIG_H

#ifndef QNUT_NO_WIRELESS
#include "cabstractwifinetconfigdialog.h"
#include "ui_adhocconfig.h"

namespace qnut {
	/**
	 * @brief CAdhocConfig provides a dialog to configure a managed ad-hoc nework or add a new ad-hoc network for the given wpa_supplicant.
	 * @author Oliver Groß <z.o.gross@gmx.de>
	 * 
	 * On creation, the CAdhocConfig sets up the user interface according to the given instance of a wpa_supplicant.
	 * The class provides function to execute the dialog for adding (w/o scan result) and configuring a network (by network id).
	 *
	 * By accepting the settings made in the UI they are verified (on error the dialog stays open).
	 */
	class CAdhocConfig : public CAbstractWifiNetConfigDialog {
		Q_OBJECT
	public:
		/**
		 * @brief Creates the object and initializes the basic user interface.
		 * @param parent parent widget
		 */
		CAdhocConfig(libnutwireless::CWireless * interface, QWidget * parent = 0);
	protected:
		Ui::ahconf ui;
	protected slots:
		void setAuthConfig(int type);
		
		virtual bool applyConfiguration();
		
		void countPskChars(QString psk);
		void togglePlainPSK(bool show);
		
		virtual void populateUi();
	private:
		void populateErrorCodeEvaluator();
	};
}
#endif

#endif
