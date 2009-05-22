#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


#include <IOKit/IOLib.h>
#include "messageType.h"
#include "simpleBase.h"
#include "cookie.h"



class Message : public SimpleBase
{
public:
	char *buffer;
	char embededBuffer[5];
	
	UInt16 getLength(){ return (buffer) ? *(UInt16*)buffer : *(UInt16*)embededBuffer;}
	UInt16 getType(){ return (buffer) ? *(UInt16*)(buffer + sizeof(UInt16)) : *(UInt16*)(embededBuffer + sizeof(UInt16)) ;}
	
	void setLength(UInt16 length)
	{
		if(buffer)
			*(UInt16*)buffer = length;
		else
			*(UInt16*)embededBuffer = length;
		
	}
	void setType(UInt16 type)
	{
		if(buffer)
			*(UInt16*)(buffer + sizeof(UInt16)) = type;
		else
			*(UInt16*)(embededBuffer + sizeof(UInt16)) = type;
	}
	
	char* getBuffer(){ return (buffer) ? buffer : embededBuffer; }
	
	virtual void free();
	//helper for debug
	void IOLog();

	static Message *createText(const char* format,...);
	static Message *createFirewallClose();//TODO: static
	static Message *createTextFromCookie(const char* message, SocketCookie* cookie);
};


#endif
