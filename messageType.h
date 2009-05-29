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
	
	void init(UInt16 size, UInt16 type){this->size = size, this->type = type;}
};

struct MessageText : public MessageBase
{
	char textBuffer[1];
};

struct MessageAskRule : public MessageBase
{
	UInt16 process_name_offset;//0 for all
	UInt16 file_path_offset;//0 for all
	
	UInt16 sock_domain;//0 for all
	UInt16 sock_type;//0 for all
	UInt16 sock_protocol;// 0 fro all	
	UInt16 sockadress_offset;// 0 for all
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	UInt8 allow;//0 denny, 1 allow
	
	pid_t pid;
	uid_t uid;
};

struct MessageRuleAdded : public MessageBase
{
	UInt32 id;
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
	void init(){ MessageBase::init(4, MessageTypeFirewallClosed);};
};



#endif
