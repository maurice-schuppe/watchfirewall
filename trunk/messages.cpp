
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
	Message* message =(Message*) ::new char[neededSize + sizeof(Message)];
	if(message)
		message->m.size = neededSize + sizeof(MessageBase);
	
	return message;
}


Message*
Message::createTextFromCookie(const char* message, SocketCookie* cookie)
{
	return createText("%11s %20s  so: %9d  pid: %3d  uid: %3d  domain: %3d  type: %3d  protocol: %3d", 
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
Message::createFirewallClose()
{
	Message *message = new(0) Message();
	if(message)
	{
		message->m.type = MessageTypeFirewallClosed; 
		message->references = 1;
	}
	return message;
}

