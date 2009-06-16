/*
 *  messageType.h
 *  Watch
 *
 *  Created by JanBird on 5/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef WATCH_MESSAGES_TYPE_H
#define WATCH_MESSAGES_TYPE_H

#include <sys/socket.h>

enum MessagesClass
{
	MessageClassInfoRule	= 0x0100,
	MessageClassInfoSocket	= 0x0200,
	MessageClassAsk			= 0x0400,
	MessageClassFirewall	= 0x0800,
	MessageClassClient		= 0x1000,
	MessageClassCommon		= 0x2000
};

enum ServerMessagesType 
{
	MessageTypeText						= MessageClassCommon | 0x01, //dymmu
	MessageTypeAskRule					= MessageClassAsk |  0x01,
	
	MessageTypeRuleAdded				= MessageClassInfoRule | 0x01,
	MessageTypeRuleDeleted				= MessageClassInfoRule | 0x02,
	MessageTypeRuleDeactivated			= MessageClassInfoRule | 0x03,
	MessageTypeRuleActivated			= MessageClassInfoRule | 0x04,
	
	MessageTypeSocketDataIN				= MessageClassInfoSocket | 0x07,
	MessageTypeSocketDataOUT			= MessageClassInfoSocket | 0x08,
	MessageTypeSocketOpen				= MessageClassInfoSocket | 0x09,
	MessageTypeSocketClosed				= MessageClassInfoSocket | 0x0A,
	MessageTypeSocketAskRule			= MessageClassInfoSocket | 0x0B,
	
	MessageTypeFirewallActivated		= MessageClassFirewall | 0x01,
	MessageTypeFirewallDeactivated		= MessageClassFirewall | 0x02,
	
	MessageTypeRegistredForAsk			= MessageClassFirewall | 0x03,
	MessageTypeUnregistredAsk			= MessageClassFirewall | 0x04,
	MessageTypeRegistredForInfoRule		= MessageClassFirewall | 0x05,
	MessageTypeUnregistredInfoRule		= MessageClassFirewall | 0x06,
	MessageTypeRegistredForInfoSocket	= MessageClassFirewall | 0x07,
	MessageTypeUnregistredInfoSocket	= MessageClassFirewall | 0x08,

	MessageTypeFirewallClosing			= MessageClassFirewall | 0x09
};

enum ClientMessagesType
{
	MessageTypeAddRule					= MessageClassClient | 0x01,
	MessageTypeDeleteRule				= MessageClassClient | 0x02,
	MessageTypeActivateRule				= MessageClassClient | 0x03,
	MessageTypeDeactivateRule			= MessageClassClient | 0x04,

	MessageTypeActivateFirewall			= MessageClassClient | 0x05,
	MessageTypeDeactivateFirewall		= MessageClassClient | 0x06,

	MessageTypeRegisterForAsk			= MessageClassClient | 0x07,
	MessageTypeUnregisterAsk			= MessageClassClient | 0x08,
	MessageTypeRegisterForInfoRule		= MessageClassClient | 0x09,
	MessageTypeUnregisterInfoRule		= MessageClassClient | 0x0A,
	MessageTypeRegisterForInfoSocket	= MessageClassClient | 0x0B,
	MessageTypeUnregisterInfoSocket		= MessageClassClient | 0x0C
};

struct MessageBase 
{
	UInt16 size;
	UInt16 type;
	
	inline void init(UInt16 size, UInt16 type){this->size = size, this->type = type;}
};

struct MessageClientAction: public MessageBase
{
	UInt32 messageId;
};

struct MessageClientActionResponce : public MessageBase
{
	UInt32 unitId;
	UInt32 messageId;
	UInt32 actionState;
	
public:
	void init(UInt32 unitId, UInt32 messageId, UInt32 actionState)
	{
		this->unitId = unitId;
		this->messageId = messageId;
		this->actionState = actionState;
	}
};

struct MessageText : public MessageBase
{
	char textBuffer[4];
};

struct MessageAskRule : public MessageBase
{
	UInt16 processNameOffset;//0 for all
	UInt16 filePathOffset;//0 for all
	
	UInt16 sockDomain;//0 for all
	UInt16 sockType;//0 for all
	UInt16 sockProtocol;// 0 fro all	
	UInt16 sockAddressOffset;// 0 for all
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	
	pid_t pid;
	uid_t uid;
	
	char buffer[4];
};

struct MessageRuleAdded : public MessageClientActionResponce
{
	UInt32 id;//rule
	
	inline void init(UInt32 id){ MessageBase::init(8, MessageTypeRuleAdded); this->id = id;}
};

struct MessageRuleDeleted : public MessageBase
{
	UInt32 id;//rule
};

struct MessageRuleDeactivated	: public MessageClientActionResponce
{
	UInt32 id;//rule
};

struct MessageRuleActivated : public MessageClientActionResponce
{
	UInt32 id;//rule
};

struct MessageRegistredForAsk : public MessageClientActionResponce
{
	
};

struct MessageUnregistredAsk : public MessageClientActionResponce
{
	
};

struct MessageRegistredForInfoRule : public MessageClientActionResponce
{
	
};

struct MessageUnregistredInfoRule : public MessageClientActionResponce
{
	
};

struct MessageRegistredForInfoSocket : public MessageClientActionResponce
{
	
};

struct MessageUnregistredInfoSocket : public MessageClientActionResponce
{
	
};

struct MessageFirewallActivated : public MessageClientActionResponce
{	
};

struct MessageFirewallDeactivated
{
};

struct MessageFirewallClosing : public MessageBase 
{
	inline void init(){ MessageBase::init(4, MessageTypeFirewallClosing);};
};

/////
struct MessageSocketDataIN : public MessageBase
{
	UInt8 stateOperation;
	UInt32 stateByRuleId;
	pid_t pid;
	uid_t uid;
	//socket_t socket;
	UInt32 packets;
	UInt32 bytes;
	sockaddr fromAddressOffset;
	sockaddr toAddressOffset;
	char processName[4];
};
struct MessageSocketDataOUT : public MessageBase
{
	
};
struct MessageSocketOpen : public MessageBase
{
	
};
struct MessageSocketClosed : public MessageBase
{
	
};


#pragma mark Client messages
struct MessageAddRule : public MessageClientAction 
{
	UInt32 id;
	UInt16 processNameOffset;//0 for all
	UInt16 filePathOffset;//0 for all
	
	UInt16 sockDomain;//0 for all
	UInt16 sockType;//0 for all
	UInt16 sockProtocol;// 0 fro all	
	UInt16 sockAddressOffset;// 0 for all
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	UInt8 allow;//0 denny, 1 allow
	UInt8 state;
	char buffer[1];
	
	char *getProcessName(){ return (char*)this + processNameOffset; }
	char *getFilePath(){ return (char*)this + filePathOffset;}
	sockaddr *getSockAddress(){ return (sockaddr*) ((sockAddressOffset) ? (char*)this + sockAddressOffset : NULL);}
};

struct MessageDeleteRule : public MessageClientAction 
{
	UInt32 id;
};

struct MessageActivateRule : public MessageClientAction 
{
	UInt32 id;
};

struct MessageDeactivateRule : public MessageClientAction 
{
	UInt32 id;
};



struct MessageActivateFirewall : public MessageClientAction 
{
};


struct MessageDeactivateFirewall : public MessageClientAction 
{
};



struct MessageRegisterForAsk : public MessageClientAction 
{
};


struct MessageUnregisterAsk : public MessageClientAction 
{
};


struct MessageRegisterForInfoRule : public MessageClientAction 
{
};


struct MessageUnregisterInfoRule : public MessageClientAction 
{
};


struct MessageRegisterForInfoSocket : public MessageClientAction 
{
};


struct MessageUnregisterInfoSocket : public MessageClientAction 
{
};




#endif
