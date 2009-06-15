#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


#include <IOKit/IOLib.h>
#include "messageType.h"
#include "simpleBase.h"
#include "cookie.h"


class Message : public SimpleBase
{
public:
	MessageBase m;

	static void *operator new(size_t size, UInt16 neededSize);

	static Message *createText(const char* format,...);
	static Message *createFirewallClosing();
	static Message *createRegistredForInfoRule();
	static Message *createUnregistredInfoRule();
	static Message *createRegistredForInfoSocket();
	static Message *createUnregistredInfoSocket();
	static Message *createRegistredForAsk();
	static Message *createUnregistredAsk();
	
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
