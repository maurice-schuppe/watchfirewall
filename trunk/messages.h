#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


#include <IOKit/IOLib.h>
#include "messageType.h"
#include "simpleBase.h"
#include "cookie.h"


class __attribute__((visibility("hidden"))) Message : public SimpleBase
{
public:
	MessageBase m;

	static void *operator new(size_t size, UInt16 neededSize);

	static Message *createText(const char* format,...);
	static Message *createFirewallClosing();
	static Message *createRegistredForInfoRule(UInt32 unitId, UInt32 messageId, UInt32 actionState);
	static Message *createUnregistredInfoRule(UInt32 unitId, UInt32 messageId, UInt32 actionState);
	static Message *createRegistredForInfoSocket(UInt32 unitId, UInt32 messageId, UInt32 actionState);
	static Message *createUnregistredInfoSocket(UInt32 unitId, UInt32 messageId, UInt32 actionState);
	static Message *createRegistredForAsk(UInt32 unitId, UInt32 messageId, UInt32 actionState);
	static Message *createUnregistredAsk(UInt32 unitId, UInt32 messageId, UInt32 actionState);
	
	static Message *createFirewallActivated(UInt32 unitId, UInt32 messageId, UInt32 actionState);
	static Message *createFirewallDeactivated(UInt32 unitId, UInt32 messageId, UInt32 actionState);
	
	static Message *createTextFromCookie(const char* message, SocketCookie* cookie);
	
	static Message *createAskRule(	
								  char* processName,
								  char* filePath,
								  
								  UInt16 sockDomain,
								  UInt16 sockType,
								  UInt16 sockProtocol,	
								  sockaddr* sockAddress,
								  
								  UInt8 direction,
								  
								  pid_t pid,
								  uid_t uid
	);
};


#endif
