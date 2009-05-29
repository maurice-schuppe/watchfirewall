/*
 *  message.cpp
 *  Watch
 *
 *  Created by JanBird on 5/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "messages.h"

void
Message::IOLog()
{
	::IOLog("message: %p; length: %d; type: %d; ref: %d \n", this, this->size, this->type, this->references);
}

void 
Message::free()
{
	if(buffer)
		delete buffer;
	
	SimpleBase::free();
}

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
	
    message->size = vsnprintf(message->buffer, 254, format, argList) + 1 + 2 * sizeof(UInt16);
	message->type = MessageTypeText;
	message->references = 1;

    va_end (argList);
	
    return message;
}


void*
Message::operator new(size_t size, UInt16 neededSize)
{
	Message* message =(Message*) ::new char[neededSize + offsetof(Message,buffer)];
	if(message)
		message->size = neededSize;
	
	return message;
}


Message*
Message::createTextFromCookie(const char* message, SocketCookie* cookie)
{
	return createText("%11s %15s  so: %9d  pid: %4d  uid: %4d  domain: %4d  type: %4d  protocol: %4d", 
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
		message->type = MessageTypeFirewallClosed;
		message->references = 1;
	}
	return message;
}

