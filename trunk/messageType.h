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

struct RawMessageBase 
{
	UInt16 size;
	UInt16 type;
	
	inline void Init(UInt16 size, UInt16 type){this->size = size, this->type = type;}
};

struct RawMessageClientAction : public RawMessageBase
{
	UInt32 messageId;
};

struct RawMessageClientActionResponce : public RawMessageBase
{
	UInt32 unitId;
	UInt32 clientMessageId;
	UInt32 actionState;
	
public:
	inline void Init(UInt16 size, UInt16 type, UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{
		RawMessageBase::Init(size, type);
		this->unitId = unitId;
		this->clientMessageId = clientMessageId;
		this->actionState = actionState;
	}
};

struct RawMessageText : public RawMessageBase
{
	char textBuffer[4];
};

struct RawMessageAskRule : public RawMessageBase
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

struct RawMessageRuleAdded : public RawMessageClientActionResponce
{
	UInt32 ruleId;//rule
	
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState , UInt32 ruleId)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRuleAdded), MessageTypeRuleAdded, unitId, clientMessageId, actionState); 
		this->ruleId = ruleId;
	}
};

struct RawMessageRuleDeleted : public RawMessageClientActionResponce
{
	UInt32 ruleId;//rule

	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState , UInt32 ruleId)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRuleDeleted), MessageTypeRuleDeleted, unitId, clientMessageId, actionState); 
		this->ruleId = ruleId;
	}
};

struct RawMessageRuleDeactivated	: public RawMessageClientActionResponce
{
	UInt32 ruleId;//rule
	
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState , UInt32 ruleId)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRuleDeactivated), MessageTypeRuleDeactivated, unitId, clientMessageId, actionState); 
		this->ruleId = ruleId;
	}
};

struct RawMessageRuleActivated : public RawMessageClientActionResponce
{
	UInt32 ruleId;//rule

	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState , UInt32 ruleId)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRuleActivated), MessageTypeRuleActivated, unitId, clientMessageId, actionState); 
		this->ruleId = ruleId;
	}
};

struct RawMessageRegistredForAsk : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRegistredForAsk), MessageTypeRegistredForAsk, unitId, clientMessageId, actionState); 
	}
};

struct RawMessageUnregistredAsk : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageUnregistredAsk), MessageTypeUnregistredAsk, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageRegistredForInfoRule : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRegistredForInfoRule), MessageTypeRegistredForInfoRule, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageUnregistredInfoRule : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageUnregistredInfoRule), MessageTypeUnregisterInfoRule, unitId, clientMessageId, actionState); 
	}	
};


//////
struct RawMessageRegistredForInfoSocket : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRegistredForInfoSocket), MessageTypeRegistredForInfoSocket, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageUnregistredInfoSocket : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageUnregistredInfoSocket), MessageTypeUnregisterInfoSocket, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageFirewallActivated : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageFirewallActivated), MessageTypeFirewallActivated, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageFirewallDeactivated : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageFirewallDeactivated), MessageTypeFirewallDeactivated, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageFirewallClosing : public RawMessageBase 
{
	inline void Init(){ RawMessageBase::Init(4, MessageTypeFirewallClosing);};
};

/////
struct RawMessageSocketDataIN : public RawMessageBase
{
	UInt8 stateOperation;
	UInt32 stateByRuleId;
	pid_t pid;
	uid_t uid;
	//socket_t socket;
	UInt32 packets;
	UInt32 bytes;
	sockaddr fromAddressOffset;//static
	sockaddr toAddressOffset;//static
	char processName[4];//static
};
struct RawMessageSocketDataOUT : public RawMessageBase
{
	
};
struct RawMessageSocketOpen : public RawMessageBase
{
	
};
struct RawMessageSocketClosed : public RawMessageBase
{
	
};


#pragma mark Client messages
struct RawMessageAddRule : public RawMessageClientAction 
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
	
	char *GetProcessName(){ return (char*)this + processNameOffset; }
	char *GetFilePath(){ return (char*)this + filePathOffset;}
	sockaddr *GetSockAddress(){ return (sockaddr*) ((sockAddressOffset) ? (char*)this + sockAddressOffset : NULL);}
};

struct RawMessageDeleteRule : public RawMessageClientAction 
{
	UInt32 ruleId;
};

struct RawMessageActivateRule : public RawMessageClientAction 
{
	UInt32 ruleId;
};

struct RawMessageDeactivateRule : public RawMessageClientAction 
{
	UInt32 ruleId;
};



struct RawMessageActivateFirewall : public RawMessageClientAction 
{
};


struct RawMessageDeactivateFirewall : public RawMessageClientAction 
{
};



struct RawMessageRegisterForAsk : public RawMessageClientAction 
{
};


struct RawMessageUnregisterAsk : public RawMessageClientAction 
{
};


struct RawMessageRegisterForInfoRule : public RawMessageClientAction 
{
};


struct RawMessageUnregisterInfoRule : public RawMessageClientAction 
{
};


struct RawMessageRegisterForInfoSocket : public RawMessageClientAction 
{
};


struct RawMessageUnregisterInfoSocket : public RawMessageClientAction 
{
};




#endif
