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
	MessageTypeDeleteRule			= MessageClassClient | 0x01,
	MessageTypeAddRule				= MessageClassClient | 0x02,
	MessageTypeActivateFirewall		= MessageClassClient | 0x03,
	MessageTypeDeactivateFirewall	= MessageClassClient | 0x04,
	MessageTypeRegisterForAsk		= MessageClassClient | 0x05,
	MessageTypeUnregisterAsk		= MessageClassClient | 0x06,
	MessageTypeRegisterForInfo		= MessageClassClient | 0x07,
	MessageTypeUnregisterInfo		= MessageClassClient | 0x08
};

#endif
