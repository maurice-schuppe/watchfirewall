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
	::IOLog("message: %p; length: %d; type: %d; ref: %d \n", this, this->length(), this->type(), this->references);
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
	*(UInt16*)(ch + sizeof(UInt16)) = MESSAGE_TEXT;
	message->references = 1;
	message->buffer = ch;
	//TODO: check length
    va_end (argList);
	
    return message;
}

Message*
Message::createClose()
{
	Message *message = new Message();
	//message->type = MESSAGE_INFO_CLOSED;
	//message->length = sizeof(Message);
	message->references = 1;
	
	return message;
}

