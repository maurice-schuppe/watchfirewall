#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


#include <IOKit/IOLib.h>
#include "messageType.h"
#include "simpleBase.h"
#include "cookie.h"


class __attribute__((visibility("hidden"))) Message : public SimpleBase
{
public:
	
	virtual void * getRawMessage() = 0;
	virtual size_t getRawMessageSize() = 0;
	
//	RawMessageBase m;
//
//	static void *operator new(size_t size, UInt16 neededSize);
//
//	static Message *createText(const char* format,...);
//	static Message *createFirewallClosing();
//	static Message *createRegistredForInfoRule(UInt32 unitId, UInt32 messageId, UInt32 actionState);
//	static Message *createUnregistredInfoRule(UInt32 unitId, UInt32 messageId, UInt32 actionState);
//	static Message *createRegistredForInfoSocket(UInt32 unitId, UInt32 messageId, UInt32 actionState);
//	static Message *createUnregistredInfoSocket(UInt32 unitId, UInt32 messageId, UInt32 actionState);
//	static Message *createRegistredForAsk(UInt32 unitId, UInt32 messageId, UInt32 actionState);
//	static Message *createUnregistredAsk(UInt32 unitId, UInt32 messageId, UInt32 actionState);
//	
//	static Message *createFirewallActivated(UInt32 unitId, UInt32 messageId, UInt32 actionState);
//	static Message *createFirewallDeactivated(UInt32 unitId, UInt32 messageId, UInt32 actionState);
//	
//	static Message *createRuleAdded(UInt32 unitId, UInt32 messageId, UInt32 actionstate, Rule *rule);
//	static Message *createRuleDeleted(UInt32 unitId, UInt32 messageId, UInt32 actionstate, UInt32 ruleId);
//	static Message *createRuleDeactivated(UInt32 unitId, UInt32 messageId, UInt32 actionstate, UInt32 ruleId);
//	static Message *createRuleActivated(UInt32 unitId, UInt32 messageId, UInt32 actionstate, UInt32 ruleId);
//	
//	static Message *createSocketDataIN(UInt32 unitId, UInt32 messageId, UInt32 actionstate);
//	static Message *createSocketDataOUT(UInt32 unitId, UInt32 messageId, UInt32 actionstate);
//	static Message *createSocketOpen(UInt32 unitId, UInt32 messageId, UInt32 actionstate);
//	static Message *createSocketClosed(UInt32 unitId, UInt32 messageId, UInt32 actionstate);
//	
//	
//	
//	
//	static Message *createTextFromCookie(const char* message, SocketCookie* cookie);
//	
//	static Message *createAskRule(	
//								  char* processName,
//								  char* filePath,
//								  
//								  UInt16 sockDomain,
//								  UInt16 sockType,
//								  UInt16 sockProtocol,	
//								  sockaddr* sockAddress,
//								  
//								  UInt8 direction,
//								  
//								  pid_t pid,
//								  uid_t uid
//	);
};

class __attribute__((visibility("hidden"))) MessageText : public Message
{
public:
	RawMessageText rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageText* Create(const char* format,...);
	static MessageText* CreateFromCookie(const char* message, SocketCookie* cookie);

};

class __attribute__((visibility("hidden"))) MessageAskRule : public Message
{
public:
	RawMessageAskRule rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageAskRule* Create(char* processName, char* filePath, UInt16 sockDomain, UInt16 sockType, UInt16 sockProtocol,	sockaddr* sockAddress, UInt8 direction,
									  pid_t pid, uid_t uid);
	
};

class __attribute__((visibility("hidden"))) MessageRuleAdded : public Message
{
public:
	RawMessageRuleAdded rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageRuleAdded* Create(UInt32 unitId, UInt32 messageId, UInt32 actionstate, UInt32 ruleId);
	
};

class __attribute__((visibility("hidden"))) MessageRuleDeleted : public Message
{
public:
	RawMessageRuleDeleted rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageRuleDeleted* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	
};

class __attribute__((visibility("hidden"))) MessageRuleDeactivated : public Message
{
public:
	RawMessageRuleDeactivated rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageRuleDeactivated* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	
};

class __attribute__((visibility("hidden"))) MessageRuleActivated : public Message
{
public:
	RawMessageRuleActivated rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageRuleActivated* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	
};

class __attribute__((visibility("hidden"))) MessageSocketDataIN	: public Message
{
public:
	RawMessageText rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageSocketDataIN* Create();
	
};

class __attribute__((visibility("hidden"))) MessageSocketDataOUT : public Message
{
public:
	RawMessageSocketDataOUT rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageSocketDataOUT* Create();
	
};

class __attribute__((visibility("hidden"))) MessageSocketOpen : public Message
{
public:
	RawMessageSocketOpen rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageSocketOpen* Create();
	
};

class __attribute__((visibility("hidden"))) MessageSocketClosed : public Message
{
public:
	RawMessageSocketClosed rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageSocketClosed* Create();
	
};

class __attribute__((visibility("hidden"))) MessageFirewallActivated : public Message
{
public:
	RawMessageFirewallActivated rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageFirewallActivated* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageFirewallDeactivated : public Message
{
public:
	RawMessageFirewallDeactivated rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageFirewallDeactivated* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageRegistredForAsk : public Message
{
public:
	RawMessageRegistredForAsk rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageRegistredForAsk* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageUnregistredAsk : public Message
{
public:
	RawMessageUnregistredAsk rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageUnregistredAsk* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageRegistredForInfoRule : public Message
{
public:
	RawMessageRegistredForInfoRule rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageRegistredForInfoRule* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageUnregistredInfoRule : public Message
{
public:
	RawMessageUnregistredInfoRule rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageUnregistredInfoRule* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageRegistredForInfoSocket : public Message
{
public:
	RawMessageRegistredForInfoSocket rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageRegistredForInfoSocket* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageUnregistredInfoSocket : public Message
{
public:
	RawMessageUnregistredInfoSocket rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageUnregistredInfoSocket* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageFirewallClosing : public Message
{
public:
	RawMessageFirewallClosing rawMessage;
	
	void * getRawMessage() { return &rawMessage; };
	size_t getRawMessageSize() { return rawMessage.size; };
	
	static MessageFirewallClosing* Create();
	
};














#endif
