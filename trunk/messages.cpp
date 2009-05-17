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
	::IOLog("message: %p; length: %d; type: %d; ref: %d \n", this, this->getLength(), this->getType(), this->references);
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
	char *ch = new char[256];
	Message *message = new Message();
	
	if(!ch)
		return NULL;
	
    *((UInt16*) ch) = vsnprintf(ch + 2 * sizeof(UInt16), 256 - 2 * sizeof(UInt16) - 1, format, argList) + 1 + 2 * sizeof(UInt16);
	*(UInt16*)(ch + sizeof(UInt16)) = MessageTypeText;
	message->references = 1;
	message->buffer = ch;
	//TODO: check length
    va_end (argList);
	
    return message;
}

Message*
Message::createFirewallClose()
{
	::IOLog("begin create firewall close \n");
	Message *message = new Message();
	if(message)
	{
		::IOLog("message buffer %p \n", message->buffer);
		message->setType(MessageTypeFirewallClosed);
		message->setLength(4);
		message->references = 1;
	}
	::IOLog("end create firewall close \n");
	return message;
}

