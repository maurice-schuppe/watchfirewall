#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


#include <IOKit/IOLib.h>
#include "messageType.h"
#include "simpleBase.h"



class Message : public SimpleBase
{
public:
	//UInt16 length;
	//UInt16 type;
	char *buffer;
	
	UInt16 length(){ return (buffer) ? *(UInt16*)buffer : 0;}
	UInt16 type(){ return (buffer) ? *(UInt16*)(buffer + sizeof(UInt16)) : 0;}
	
	virtual void free();
	//helper for debug
	void IOLog();

	static Message *createText(const char* format,...);
	static Message *createClose();//TODO: static
};


#endif
