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
#include <sys/kauth.h>

#include "simpleBase.h"

class Application: public SimpleBase
{
public:
	static Application *applications;
	static IOLock *lock;
	
	static kauth_listener_t processListener;

public:
	static bool initStatic()
	{
		lock = IOLockAlloc();
		if(!lock)
			return false;
		
		processListener = kauth_listen_scope(KAUTH_SCOPE_FILEOP, callbackProcessListener, NULL);
		
		if(!processListener)
		{
			freeStatic();
			return false;
		}
		
		return true;
	}
	
	static void freeStatic()
	{
		if(processListener)
			kauth_unlisten_scope(processListener);
		
		processListener = NULL;

		if(lock)
			IOLockFree(lock);
		
		lock = NULL;
	}
	
	static Application* getApplication(); 
	static Application* addApplication(vnode_t vnode, const char* filePath);
	static int callbackProcessListener
					(
					   kauth_cred_t    credential,
					   void *          idata,
					   kauth_action_t  action,
					   uintptr_t       arg0,
					   uintptr_t       arg1,
					   uintptr_t       arg2,
					   uintptr_t       arg3
					 );
	
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
		//::IOLog("application deleted pid: %d\n", this->pid);
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