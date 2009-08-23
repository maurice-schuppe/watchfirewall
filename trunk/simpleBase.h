#ifndef WATCH_SIMPLE_BASE_H
#define WATCH_SIMPLE_BASE_H

#include <IOKit/IOLib.h>

class __attribute__((visibility("hidden"))) SimpleBase
{
public:
	SInt32 references;
	
public:
	//void IOLog();
	
	int Retain();
	int Release();
	virtual void Free();
};


#endif WATCH_SIMPLE_BASE_H