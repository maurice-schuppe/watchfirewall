/*
 *  SimpleBase.h
 *  Watch
 *
 *  Created by JanBird on 5/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef WATCH_SIMPLE_BASE_H
#define WATCH_SIMPLE_BASE_H

#include <IOKit/IOLib.h>

class __attribute__((visibility("hidden"))) SimpleBase
{
public:
	SInt32 references;
	
public:
	void IOLog();
	
	int retain();
	int release();
	virtual void free();
};


#endif WATCH_SIMPLE_BASE_H