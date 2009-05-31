/*
 *  application.h
 *  Watch
 *
 *  Created by JanBird on 5/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef APPLICATION_WATCH_H
#define APPLICATION_WATCH_H

#include <libkern/c++/OSString.h>
#include <IOKit/IOLocks.h>

#include "simpleBase.h"

class Application: public SimpleBase
{
public:
	static Application *applications;
	static IOLock *lock;

public:
	static bool initStatic()
	{
		lock = IOLockAlloc();
		if(!lock)
			return false;
		
		return true;
	}
	
	static void freeStatic()
	{
		if(lock)
			IOLockFree(lock);
		
		lock = NULL;
	}
	
	static Application* getApplication();
	
public:
	pid_t pid;
	pid_t p_pid;
	uid_t uid;	
	OSString *processName;
	OSString *filePath;
	
	Application *prev;
	Application *next;

	virtual void free()
	{
		::IOLog("application deleted pid: %d\n", this->pid);
		if(prev)
			prev->next = next;
		
		if(next)
			next->prev = prev;
		
		prev = next = NULL;
		
		if(this == applications)
			applications = NULL;
		
		if(filePath)
			filePath->release();
		
		if(processName)
			processName->release();
		
		SimpleBase::free();
	}
	
	int release()
	{
		IOLockLock(lock);
		int result = SimpleBase::release();
		IOLockUnlock(lock);
		
		return result;
	}
	
};


#endif 