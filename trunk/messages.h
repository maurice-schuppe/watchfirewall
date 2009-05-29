#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


#include <IOKit/IOLib.h>
#include "messageType.h"
#include "simpleBase.h"
#include "cookie.h"

class MessageWraper
{
public:
	char *buffer;
	
	UInt16 getSize(){ return *((UInt16*)buffer);}
	UInt16 getType (){ return *((UInt16*)buffer + 1);}
};

class Message : public SimpleBase
{
public:
	UInt16 size;
	UInt16 type;
	char buffer[5];
	
	
	virtual void free();
	static void *operator new(size_t size, UInt16 neededSize);
	
	//helper for debug
	void IOLog();

	static Message *createText(const char* format,...);
	static Message *createFirewallClose();//TODO: static
	static Message *createTextFromCookie(const char* message, SocketCookie* cookie);
};


#endif
