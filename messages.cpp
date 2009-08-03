
#include "messages.h"
#include "messageType.h"


MessageText*
MessageText::Create(const char* format,...)
{
    va_list argList;
	
    if (!format)
        return 0;
	
    va_start(argList, format);
	MessageText *message = (MessageText*)new char[255 + sizeof(RawMessageBase) + sizeof(Message)];
	
	if(!message)
		return NULL;
	
    message->rawMessage.size = vsnprintf((char*)(message->rawMessage.textBuffer), 254, format, argList) + sizeof(RawMessageBase) + 1;
	message->rawMessage.type = MessageTypeText;
	message->references = 1;

    va_end (argList);
	
    return message;
}

//void*
//Message::operator new(size_t size, UInt16 neededSize)
//{
//	Message* message =(Message*) ::new char[neededSize + sizeof(Message) - sizeof(RawMessageBase)];
//	if(message)
//		message->m.size = neededSize;
//	
//	return message;
//}

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

MessageRegistredForInfoRule*
MessageRegistredForInfoRule::Create(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	MessageRegistredForInfoRule *message = new MessageRegistredForInfoRule;
	if(message)
	{
		message->rawMessage.init(unitId, messageId, actionState);
		message->rawMessage.type = MessageTypeRegistredForInfoRule; 
		message->references = 1;
	}
	return message;
}

MessageUnregistredInfoRule*
MessageUnregistredInfoRule::Create(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	MessageUnregistredInfoRule *message = new MessageUnregistredInfoRule;
	if(message)
	{
		message->rawMessage.init(unitId, messageId, actionState);
		message->rawMessage.type = MessageTypeUnregistredInfoRule; 
		message->references = 1;
	}
	return message;
}

MessageRegistredForInfoSocket*
MessageRegistredForInfoSocket::Create(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	MessageRegistredForInfoSocket *message = new MessageRegistredForInfoSocket;
	if(message)
	{
		message->rawMessage.init(unitId, messageId, actionState);
		message->rawMessage.type = MessageTypeRegistredForInfoSocket; 
		message->references = 1;
	}
	return message;
}

MessageUnregistredInfoSocket*
MessageUnregistredInfoSocket::Create(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	MessageUnregistredInfoSocket *message = new MessageUnregistredInfoSocket;
	if(message)
	{
		message->rawMessage.init(unitId, messageId, actionState);
		message->rawMessage.type = MessageTypeUnregistredInfoSocket; 
		message->references = 1;
	}
	return message;
}

MessageRegistredForAsk*
MessageRegistredForAsk::Create(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	MessageRegistredForAsk *message = new MessageRegistredForAsk;
	if(message)
	{
		message->rawMessage.init(unitId, messageId, actionState);
		message->rawMessage.type = MessageTypeRegistredForAsk; 
		message->references = 1;
	}
	return message;
}

MessageUnregistredAsk*
MessageUnregistredAsk::Create(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	MessageUnregistredAsk *message = new MessageUnregistredAsk;
	if(message)
	{
		message->rawMessage.init(unitId, messageId, actionState);
		message->rawMessage.type = MessageTypeUnregistredAsk; 
		message->references = 1;
	}
	return message;
}

MessageFirewallActivated*
MessageFirewallActivated::Create(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	MessageFirewallActivated *message = new MessageFirewallActivated;
	if(message)
	{
		message->rawMessage.type = MessageTypeFirewallActivated; 
		message->references = 1;
	}
	return message;
}

MessageFirewallDeactivated*
MessageFirewallDeactivated::Create(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	MessageFirewallDeactivated *message = new MessageFirewallDeactivated;
	if(message)
	{
		message->rawMessage.init(unitId, messageId, actionState);
		message->rawMessage.type = MessageTypeFirewallDeactivated; 
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
		message->rawMessage.type = MessageTypeFirewallClosing; 
		message->references = 1;
	}
	return message;
}


MessageRuleAdded*
MessageRuleAdded::Create(UInt32 unitId, UInt32 messageId, UInt32 actionstate, UInt32 ruleId)
{
	return NULL;
}

MessageRuleDeactivated*
MessageRuleDeactivated::Create(UInt32 unitId, UInt32 messageId, UInt32 actionstate, UInt32 ruleId)
{
	return NULL;
}

MessageRuleActivated*
MessageRuleActivated::Create(UInt32 unitId, UInt32 messageId, UInt32 actionstate, UInt32 ruleId)
{
	return NULL;
}

MessageSocketDataIN*
MessageSocketDataIN::Create()
{
	return NULL;
}

MessageSocketDataOUT*
MessageSocketDataOUT::Create()
{
	return NULL;
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

