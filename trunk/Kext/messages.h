#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


#include <IOKit/IOLib.h>
#include "messageType.h"
#include "simpleBase.h"
#include "cookie.h"


class __attribute__((visibility("hidden"))) Message : public SimpleBase
{
public:
	RawMessageBase raw;
	
	//TODO: refactor
	inline void *operator new(size_t size, size_t rawMessageSize){ return ::new char[size + rawMessageSize - sizeof(RawMessageBase)]; }
	inline void *operator new(size_t size){ return ::new char[size]; }
	
	static Message* CreateText(const char* format,...);
	static Message* CreateTextFromCookie(const char* message, SocketCookie* cookie);
	
	static Message*	CreateSfltUnregistered();//??????
	static Message*	CreateSfltAttach(socket_t so);
	static Message*	CreateSfltDetach(socket_t so);
	static Message*	CreateSfltNotify(socket_t so, UInt16 event);
	static Message*	CreateSfltGetPeerName(socket_t so, sockaddr *sa);
	static Message*	CreateSfltGetSockName(socket_t so, sockaddr *sa);
	static Message*	CreateSfltDataIn(socket_t so, const sockaddr *from);
	static Message*	CreateSfltDataOut(socket_t so, const sockaddr *to);
	static Message*	CreateSfltConnectIn(socket_t so, const sockaddr *from);
	static Message*	CreateSfltConnectOut(socket_t so, const sockaddr *to);
	static Message*	CreateSfltBind(socket_t so, const sockaddr *to);
	static Message*	CreateSfltSetOption(socket_t so, sockopt_t opt);
	static Message*	CreateSfltGetOption(socket_t so, sockopt_t opt);
	static Message*	CreateSfltListen(socket_t so);
	static Message*	CreateSfltIoctl(socket_t so, UInt32 request, const char* argp);
	static Message*	CreateSfltAccept(socket_t so_listen, socket_t so, const sockaddr *local, const sockaddr *remote);
	
	
	static Message* CreateAskRule(char* processName, char* filePath, UInt16 sockDomain, UInt16 sockType, UInt16 sockProtocol, sockaddr* sockAddress, UInt8 direction, pid_t pid, uid_t uid);
	static Message* CreateRuleAdded(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	static Message* CreateRuleDeleted(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	static Message* CreateRuleActivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	static Message* CreateRuleDeactivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	static Message* CreateSocketData(UInt8 direction, UInt8 stateOperation, UInt32 stateByRuleId, pid_t pid, uid_t uid, socket_t so, UInt32 packets, UInt32 bytes, sockaddr *fromAddress, sockaddr *toAddress, OSString *processName);
	static Message* CreateSocketOpen(socket_t so);
	static Message* CreateSocketClosed(socket_t so);
	static Message* CreateFirewallActivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	static Message* CreateFirewallDeactivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
	static Message* CreateClientSubscribedAsaProviderOfRules(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	static Message* CreateClientUnsubscribedAsaProviderOfRules(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	static Message* CreateClientSubscribedToInfoRules(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	static Message* CreateClientUnsubscribedFromInfoRules(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	static Message* CreateClientSubscribedToInfoSockets(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	static Message* CreateClientUnsubscribedFromInfoSockets(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
	static Message* CreateFirewallClosing();
};

#endif
