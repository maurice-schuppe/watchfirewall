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
	MessageTypeText					= MessageClassCommon | 0x01, //dymmu
	MessageTypeAskRule				= MessageClassAsk |  0x01,
	
	MessageTypeRuleAdded			= MessageClassInfoRule | 0x01,
	MessageTypeRuleDeleted			= MessageClassInfoRule | 0x02,
	MessageTypeRuleDisabled			= MessageClassInfoRule | 0x03,
	MessageTypeRuleEnbled			= MessageClassInfoRule | 0x04,
	
	MessageTypeSocketRejected		= MessageClassInfoSocket | 0x05,
	MessageTypeSocketAllowed		= MessageClassInfoSocket | 0x06,
	MessageTypeSocketDataIN			= MessageClassInfoSocket | 0x07,
	MessageTypeSocketDataOUT		= MessageClassInfoSocket | 0x08,
	MessageTypeSocketOpen			= MessageClassInfoSocket | 0x09,
	MessageTypeSocketClosed			= MessageClassInfoSocket | 0x0A,
	MessageTypeSocketAskRule		= MessageClassInfoSocket | 0x0B,
	
	MessageTypeFirewallActivated	= MessageClassFirewall | 0x01,
	MessageTypeFirewallDeactivated	= MessageClassFirewall | 0x02,
	
	MessageTypeFirewallClosed		= MessageClassFirewall | 0x03
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

struct MessageRuleAdded : public MessageBase
{
	UInt32 id;
	
	inline void init(UInt32 id){ MessageBase::init(8, MessageTypeRuleAdded); this->id = id;}
};

struct MessageRuleDeleted : public MessageBase
{
	UInt32 id;
};

struct MessageRuleDisabled	: public MessageBase
{
	UInt32 id;
};

struct MessageRuleEnbled : public MessageBase
{
	UInt32 id;
};


struct MessageFirewallClosed : public MessageBase 
{
	inline void init(){ MessageBase::init(4, MessageTypeFirewallClosed);};
};


#pragma mark Client messages
struct MessageAddRule : public MessageBase 
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
	
	char *getProcessName(){ return (processNameOffset) ? (char*)this + processNameOffset : NULL; }
	char *getFilePath(){ return (filePathOffset) ? (char*)this + filePathOffset : NULL;}
	sockaddr *getSockAddress(){ return (sockaddr*) ((sockAddressOffset) ? (char*)this + sockAddressOffset : NULL);}
};

struct MessageDeleteRule : public MessageBase 
{
	UInt32 id;
};

struct MessageActivateRule : public MessageBase 
{
	UInt32 id;
};

struct MessageDeactivateRule : public MessageBase 
{
	UInt32 id;
};



struct MessageActivateFirewall : public MessageBase 
{
};


struct MessageDeactivateFirewall : public MessageBase 
{
};



struct MessageRegisterForAsk : public MessageBase 
{
};


struct MessageUnregisterAsk : public MessageBase 
{
};


struct MessageRegisterForInfoRule : public MessageBase 
{
};


struct MessageUnregisterInfoRule : public MessageBase 
{
};


struct MessageRegisterForInfoSocket : public MessageBase 
{
};


struct MessageUnregisterInfoSocket : public MessageBase 
{
};




#endif
