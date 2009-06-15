
#include "messages.h"

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
Message::createRegistredForInfoRule()
{
	Message *message = new(sizeof(MessageRegistredForInfoRule)) Message();
	if(message)
	{
		message->m.type = MessageTypeRegistredForInfoRule; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createUnregistredInfoRule()
{
	Message *message = new(sizeof(MessageUnregistredInfoRule)) Message();
	if(message)
	{
		message->m.type = MessageTypeUnregistredInfoRule; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createRegistredForInfoSocket()
{
	Message *message = new(sizeof(MessageRegistredForInfoSocket)) Message();
	if(message)
	{
		message->m.type = MessageTypeRegistredForInfoSocket; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createUnregistredInfoSocket()
{
	Message *message = new(sizeof(MessageUnregistredInfoSocket)) Message();
	if(message)
	{
		message->m.type = MessageTypeUnregistredInfoSocket; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createRegistredForAsk()
{
	Message *message = new(sizeof(MessageRegistredForAsk)) Message();
	if(message)
	{
		message->m.type = MessageTypeRegistredForAsk; 
		message->references = 1;
	}
	return message;
}

Message*
Message::createUnregistredAsk()
{
	Message *message = new(sizeof(MessageUnregistredAsk)) Message();
	if(message)
	{
		message->m.type = MessageTypeUnregistredAsk; 
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

