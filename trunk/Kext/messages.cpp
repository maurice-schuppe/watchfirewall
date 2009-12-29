#include "messages.h"
#include "messageType.h"


Message*
Message::CreateText(const char* format,...)
{
    va_list argList;
	
    if (!format)
        return 0;

	Message *message = new(255 + sizeof(RawMessageBase)) Message;
	
	if(!message)
		return NULL;
	
    va_start(argList, format);
	
    message->raw.size = vsnprintf((char*)(&message->raw/* - by end*/), 254, format, argList) + sizeof(RawMessageBase) + 1;

    va_end (argList);

	message->raw.type = MessageTypeText;
	message->references = 1;
	
    return message;
}

SInt32 nextTextMessage;//TODO: only for debug

Message*
Message::CreateTextFromCookie(const char* message, SocketCookie* cookie)
{
	UInt64 time;
	clock_get_uptime(&time);
	UInt32 mc = OSIncrementAtomic(&nextTextMessage);
	return CreateText("%d %lld %11s %20s path: %s  so: %9d  pid: %3d  uid: %3d  domain: %3d  type: %3d  protocol: %3d", 
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

Message*
Message::CreateAskRule(char* processName, char* filePath, UInt16 sockDomain, UInt16 sockType, UInt16 sockProtocol,	sockaddr* sockAddress, UInt8 direction,
					   pid_t pid, uid_t uid)
{
	//TODO: implement
	return NULL;
}

Message*
Message::CreateRegistredForInfoRule(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new(sizeof(RawMessageRegistredForInfoRule)) Message;
	if(message)
	{
		((RawMessageRegistredForInfoRule*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateUnregistredInfoRule(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	//TODO: size
	Message *message = new(sizeof(RawMessageUnregistredInfoRule)) Message;
	if(message)
	{
		((RawMessageUnregistredInfoRule*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateRegistredForInfoSocket(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new(sizeof(RawMessageRegistredForInfoSocket)) Message;
	if(message)
	{
		((RawMessageRegistredForInfoSocket*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateUnregistredInfoSocket(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageUnregistredInfoSocket)) Message;
	if(message)
	{
		((RawMessageUnregistredInfoSocket*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateRegistredForAsk(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageRegistredForAsk)) Message;
	if(message)
	{
		((RawMessageRegistredForAsk*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateUnregistredAsk(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageUnregistredAsk)) Message;
	if(message)
	{
		((RawMessageUnregistredAsk*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateFirewallActivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageFirewallActivated)) Message;
	if(message)
	{
		((RawMessageFirewallActivated*)&message->raw)->Init(unitId, clientMessageId, actionState); 
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateFirewallDeactivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageFirewallDeactivated)) Message;
	if(message)
	{
		((RawMessageFirewallActivated*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateFirewallClosing()
{
	Message *message = new Message();
	if(message)
	{
		RawMessageFirewallClosing* rawMessageFirewallClosing = (RawMessageFirewallClosing*)&message->raw; 
		rawMessageFirewallClosing->Init(); 
		message->references = 1;
	}
	return message;
}


Message*
Message::CreateRuleAdded(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	Message* message = new (sizeof(RawMessageRuleAdded)) Message;
	if(message)
	{
		((RawMessageRuleAdded*)&message->raw)->Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

Message* 
Message::CreateRuleDeleted(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	Message* message = new (sizeof(RawMessageRuleDeleted)) Message;
	if(message)
	{
		((RawMessageRuleDeleted*)&message->raw)->Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}


Message*
Message::CreateRuleDeactivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	Message* message = new (sizeof(RawMessageRuleDeactivated)) Message;
	if(message)
	{
		((RawMessageRuleDeactivated*)&message->raw)->Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

Message*
Message::CreateRuleActivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	Message* message = new (sizeof(RawMessageRuleActivated)) Message;
	if(message)
	{
		((RawMessageRuleActivated*)&message->raw)->Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

Message*
Message::CreateSocketData(UInt8 direction, UInt8 stateOperation, UInt32 stateByRuleId, pid_t pid, uid_t uid, socket_t so, UInt32 packets, UInt32 bytes, sockaddr *fromAddress, sockaddr *toAddress, OSString *processName)
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
	
	//TODO: is real
	Message* message = new(neddedSize) Message; 
	if(message)
	{
		((RawMessageSocketData*)&message->raw)->Init(neddedSize, direction, stateOperation, stateByRuleId, pid, uid, so, packets, bytes, fromAddress, toAddress, processNameC, processNameSize);
		message->references = 1;
	}
	
	return message;
}

Message*
Message::CreateSocketOpen(socket_t so)
{
	return NULL;
}

Message*
Message::CreateSocketClosed(socket_t so)
{
	return NULL;
}

