#ifndef WATCH_SIMPLE_BASE_H
#define WATCH_SIMPLE_BASE_H

#include <IOKit/IOLib.h>

class __attribute__((visibility("hidden"))) SimpleBase
{
public:
	SInt32 references;
	
public:
	
	int Retain() { return OSIncrementAtomic(&references); }
	int Release() 
	{
		if(OSDecrementAtomic(&references) == 1)
		{
			Free();
			return 0;
		}
		
		return references;
	}
	
	virtual void Free();	
};


#endif WATCH_SIMPLE_BASE_H