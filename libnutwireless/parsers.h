#ifndef LIBNUTWIRELESS_PARSERS
#define LIBNUTWIRELESS_PARSERS

#include <QStringList>
#include "wstypes.h"


namespace libnutwireless {
	/** @brief The parser class contains all wpa_supplicant specific parser functions
		
		The class is used to integrate all parser functions that we need later in
		CWpaSupplicantBase and CWpaSupplicant.

	*/
	class CWpaSupplicantParsers {
		protected:

		QStringList sliceMessage(QString str);
		
		//Parse MIB Variables
		MIBVariables parseMIB(QStringList list);
		MIBVariable::MIBVariable_type parseMIBType(QString str);
		
		//parse list network
		QList<ShortNetworkInfo> parseListNetwork(QStringList list);
		NetworkFlags parseNetworkFlags(QString str);
	
		//parse Status with helper functionss
		Status parseStatus(QStringList list);
		Status::WPA_STATE parseWpaState(QString str);
		Status::PAE_STATE parsePaeState(QString str);
		Status::PORT_STATUS parsePortStatus(QString str);
		Status::PORT_CONTROL parsePortControl(QString str);
		Status::BACKEND_STATE parseBackendState(QString str);
		Status::EAP_STATE parseEapState(QString str);
		Status::METHOD_STATE parseMethodState(QString str);
		Status::DECISION parseDecision(QString str);
	
		
		//parse Event
		EventType parseEvent(QString str);
		Request parseReq(QString str);
		RequestType parseReqType(QString str);
		InteractiveType parseInteract(QString str);
		int parseEventNetworkId(QString str);
	
	};
}
#endif

