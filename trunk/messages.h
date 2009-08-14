#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


#include <IOKit/IOLib.h>
#include "messageType.h"
#include "simpleBase.h"
#include "cookie.h"


class __attribute__((visibility("hidden"))) Message : public SimpleBase
{
public:
	
	virtual void * GetRawMessage() = 0;
	virtual size_t GetRawMessageSize() = 0;
	virtual UInt16 GetRawMessageType() = 0;
	
	inline static void *operator new(size_t size, UInt16 neededSize){ return ::new char[neededSize + sizeof(Message)]; }
	inline static void *operator new(size_t size){ return ::new char[size]; }
};

class __attribute__((visibility("hidden"))) MessageText : public Message
{
public:
	RawMessageText rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageText* Create(const char* format,...);
	static MessageText* CreateFromCookie(const char* message, SocketCookie* cookie);

};

class __attribute__((visibility("hidden"))) MessageAskRule : public Message
{
public:
	RawMessageAskRule rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageAskRule* Create(char* processName, char* filePath, UInt16 sockDomain, UInt16 sockType, UInt16 sockProtocol,	sockaddr* sockAddress, UInt8 direction,
									  pid_t pid, uid_t uid);
	
};

class __attribute__((visibility("hidden"))) MessageRuleAdded : public Message
{
public:
	RawMessageRuleAdded rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageRuleAdded* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	
};

class __attribute__((visibility("hidden"))) MessageRuleDeleted : public Message
{
public:
	RawMessageRuleDeleted rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageRuleDeleted* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	
};

class __attribute__((visibility("hidden"))) MessageRuleDeactivated : public Message
{
public:
	RawMessageRuleDeactivated rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageRuleDeactivated* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	
};

class __attribute__((visibility("hidden"))) MessageRuleActivated : public Message
{
public:
	RawMessageRuleActivated rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageRuleActivated* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId);
	
};

class __attribute__((visibility("hidden"))) MessageSocketDataIN	: public Message
{
public:
	RawMessageText rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageSocketDataIN* Create();
	
};

class __attribute__((visibility("hidden"))) MessageSocketDataOUT : public Message
{
public:
	RawMessageSocketDataOUT rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageSocketDataOUT* Create();
	
};

class __attribute__((visibility("hidden"))) MessageSocketOpen : public Message
{
public:
	RawMessageSocketOpen rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageSocketOpen* Create();
	
};

class __attribute__((visibility("hidden"))) MessageSocketClosed : public Message
{
public:
	RawMessageSocketClosed rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageSocketClosed* Create();
	
};

class __attribute__((visibility("hidden"))) MessageFirewallActivated : public Message
{
public:
	RawMessageFirewallActivated rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageFirewallActivated* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageFirewallDeactivated : public Message
{
public:
	RawMessageFirewallDeactivated rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageFirewallDeactivated* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageRegistredForAsk : public Message
{
public:
	RawMessageRegistredForAsk rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageRegistredForAsk* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageUnregistredAsk : public Message
{
public:
	RawMessageUnregistredAsk rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageUnregistredAsk* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageRegistredForInfoRule : public Message
{
public:
	RawMessageRegistredForInfoRule rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageRegistredForInfoRule* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageUnregistredInfoRule : public Message
{
public:
	RawMessageUnregistredInfoRule rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageUnregistredInfoRule* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageRegistredForInfoSocket : public Message
{
public:
	RawMessageRegistredForInfoSocket rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageRegistredForInfoSocket* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageUnregistredInfoSocket : public Message
{
public:
	RawMessageUnregistredInfoSocket rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageUnregistredInfoSocket* Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState);
	
};


class __attribute__((visibility("hidden"))) MessageFirewallClosing : public Message
{
public:
	RawMessageFirewallClosing rawMessage;
	
	void * GetRawMessage() { return &rawMessage; };
	size_t GetRawMessageSize() { return rawMessage.size; };
	UInt16 GetRawMessageType() { return rawMessage.type; };
	
	static MessageFirewallClosing* Create();
	
};














#endif
