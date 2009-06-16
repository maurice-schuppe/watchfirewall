
#include "messages.h"
#include "messageType.h"


Message*
Message::createText(const char* format,...)
{
    va_list argList;
	
    if (!format)
        return 0;
	
    va_start(argList, format);
	Message *message = new(255) Message();
	
	if(!message)
		return NULL;
	
    message->m.size = vsnprintf((char*)(&message->m)  + sizeof(MessageBase), 254, format, argList) + sizeof(MessageBase) + 1;
	message->m.type = MessageTypeText;
	message->references = 1;

    va_end (argList);
	
    return message;
}

void*
Message::operator new(size_t size, UInt16 neededSize)
{
	Message* message =(Message*) ::new char[neededSize + sizeof(Message) - sizeof(MessageBase)];
	if(message)
		message->m.size = neededSize;
	
	return message;
}

SInt32 nextTextMessage;//TODO: only for debug

Message*
Message::createTextFromCookie(const char* message, SocketCookie* cookie)
{
	UInt64 time;
	clock_get_uptime(&time);
	UInt32 mc = OSIncrementAtomic(&nextTextMessage);
	return createText("%d %lld %11s %20s  so: %9d  pid: %3d  uid: %3d  domain: %3d  type: %3d  protocol: %3d", 
					  mc,
					  time,
					  message, 
					  cookie->application->processName->getCStringNoCopy(),
					  cookie->socket, 
					  cookie->application->pid, 
					  cookie->application->uid,
					  cookie->sockDomain,
					  cookie->sockType,
					  cookie->sockProtocol);
}

Message*
Message::createRegistredForInfoRule(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	Message *message = new(sizeof(MessageRegistredForInfoRule)) Message();
	if(message)
	{
		((MessageClientActionResponce*) &message->m)->init(unitId, messageId, actionState);
		message->m.type = MessageTypeRegistredForInfoRule; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createUnregistredInfoRule(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	Message *message = new(sizeof(MessageUnregistredInfoRule)) Message();
	if(message)
	{
		((MessageClientActionResponce*) &message->m)->init(unitId, messageId, actionState);
		message->m.type = MessageTypeUnregistredInfoRule; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createRegistredForInfoSocket(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	Message *message = new(sizeof(MessageRegistredForInfoSocket)) Message();
	if(message)
	{
		((MessageClientActionResponce*) &message->m)->init(unitId, messageId, actionState);
		message->m.type = MessageTypeRegistredForInfoSocket; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createUnregistredInfoSocket(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	Message *message = new(sizeof(MessageUnregistredInfoSocket)) Message();
	if(message)
	{
		((MessageClientActionResponce*) &message->m)->init(unitId, messageId, actionState);
		message->m.type = MessageTypeUnregistredInfoSocket; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createRegistredForAsk(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	Message *message = new(sizeof(MessageRegistredForAsk)) Message();
	if(message)
	{
		((MessageClientActionResponce*) &message->m)->init(unitId, messageId, actionState);
		message->m.type = MessageTypeRegistredForAsk; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createUnregistredAsk(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	Message *message = new(sizeof(MessageUnregistredAsk)) Message();
	if(message)
	{
		((MessageClientActionResponce*) &message->m)->init(unitId, messageId, actionState);
		message->m.type = MessageTypeUnregistredAsk; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createFirewallActivated(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	Message *message = new(sizeof(MessageFirewallActivated)) Message();
	if(message)
	{
		message->m.type = MessageTypeFirewallActivated; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createFirewallDeactivated(UInt32 unitId, UInt32 messageId, UInt32 actionState)
{
	Message *message = new(sizeof(MessageFirewallDeactivated)) Message();
	if(message)
	{
		((MessageClientActionResponce*) &message->m)->init(unitId, messageId, actionState);
		message->m.type = MessageTypeFirewallDeactivated; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createFirewallClosing()
{
	Message *message = new(sizeof(MessageFirewallClosing)) Message();
	if(message)
	{
		message->m.type = MessageTypeFirewallClosing; 
		message->references = 1;
	}
	return message;
}

