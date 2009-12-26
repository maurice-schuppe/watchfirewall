#include "messages.h"
#include "messageType.h"


MessageText*
MessageText::Create(const char* format,...)
{
    va_list argList;
	
    if (!format)
        return 0;

	MessageText *message = new(255 + sizeof(RawMessageBase)) MessageText;
	
	if(!message)
		return NULL;
	
    va_start(argList, format);
	
    message->rawMessage.size = vsnprintf((char*)(message->rawMessage.textBuffer), 254, format, argList) + sizeof(RawMessageBase) + 1;

    va_end (argList);

	message->rawMessage.type = MessageTypeText;
	message->references = 1;
	
    return message;
}

SInt32 nextTextMessage;//TODO: only for debug

MessageText*
MessageText::CreateFromCookie(const char* message, SocketCookie* cookie)
{
	UInt64 time;
	clock_get_uptime(&time);
	UInt32 mc = OSIncrementAtomic(&nextTextMessage);
	return Create("%d %lld %11s %20s path: %s  so: %9d  pid: %3d  uid: %3d  domain: %3d  type: %3d  protocol: %3d", 
					  mc,
					  time,
					  message, 
					  cookie->application->processName->getCStringNoCopy(),
					  cookie->application->filePath->getCStringNoCopy(),
					  cookie->socket, 
					  cookie->application->pid, 
					  cookie->application->uid,
					  cookie->sockDomain,
					  cookie->sockType,
					  cookie->sockProtocol);
}

MessageAskRule*
MessageAskRule::Create(char* processName, char* filePath, UInt16 sockDomain, UInt16 sockType, UInt16 sockProtocol,	sockaddr* sockAddress, UInt8 direction,
					   pid_t pid, uid_t uid)
{
	//TODO: implement
	return NULL;
}

MessageRegistredForInfoRule*
MessageRegistredForInfoRule::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	MessageRegistredForInfoRule *message = new MessageRegistredForInfoRule;
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

MessageUnregistredInfoRule*
MessageUnregistredInfoRule::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	MessageUnregistredInfoRule *message = new MessageUnregistredInfoRule;
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

MessageRegistredForInfoSocket*
MessageRegistredForInfoSocket::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	MessageRegistredForInfoSocket *message = new MessageRegistredForInfoSocket;
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

MessageUnregistredInfoSocket*
MessageUnregistredInfoSocket::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	MessageUnregistredInfoSocket *message = new MessageUnregistredInfoSocket;
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

MessageRegistredForAsk*
MessageRegistredForAsk::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	MessageRegistredForAsk *message = new MessageRegistredForAsk;
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

MessageUnregistredAsk*
MessageUnregistredAsk::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	MessageUnregistredAsk *message = new MessageUnregistredAsk;
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

MessageFirewallActivated*
MessageFirewallActivated::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	MessageFirewallActivated *message = new MessageFirewallActivated;
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState); 
		message->references = 1;
	}
	return message;
}

MessageFirewallDeactivated*
MessageFirewallDeactivated::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	MessageFirewallDeactivated *message = new MessageFirewallDeactivated;
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

MessageFirewallClosing*
MessageFirewallClosing::Create()
{
	MessageFirewallClosing *message = new MessageFirewallClosing();
	if(message)
	{
		message->rawMessage.Init(); 
		message->references = 1;
	}
	return message;
}


MessageRuleAdded*
MessageRuleAdded::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	MessageRuleAdded* message = new MessageRuleAdded();
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

MessageRuleDeleted* 
MessageRuleDeleted::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	MessageRuleDeleted* message = new MessageRuleDeleted();
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}


MessageRuleDeactivated*
MessageRuleDeactivated::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	MessageRuleDeactivated* message = new MessageRuleDeactivated();
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

MessageRuleActivated*
MessageRuleActivated::Create(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	MessageRuleActivated* message = new MessageRuleActivated();
	if(message)
	{
		message->rawMessage.Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

MessageSocketData*
MessageSocketData::Create(UInt8 direction, UInt8 stateOperation, UInt32 stateByRuleId, pid_t pid, uid_t uid, socket_t so, UInt32 packets, UInt32 bytes, sockaddr *fromAddress, sockaddr *toAddress, OSString *processName)
{
	//calculate size
	int neddedSize = sizeof(RawMessageSocketData);//- 3
	int processNameSize = 0;
	const char *processNameC = NULL;
	
	if(fromAddress != NULL)
		neddedSize += fromAddress->sa_len;
	
	if(toAddress != NULL)
		neddedSize += toAddress->sa_len;
	
	if(processName != NULL)
	{
		processNameSize = processName->getLength();
		processNameC = processName->getCStringNoCopy();
		neddedSize += processNameSize;
	}
	
	MessageSocketData* message = new(neddedSize) MessageSocketData; 
	if(message)
	{
		message->rawMessage.Init(neddedSize, direction, stateOperation, stateByRuleId, pid, uid, so, packets, bytes, fromAddress, toAddress, processNameC, processNameSize);
		message->references = 1;
	}
	
	return message;
}

MessageSocketOpen*
MessageSocketOpen::Create()
{
	return NULL;
}

MessageSocketClosed*
MessageSocketClosed::Create()
{
	return NULL;
}

