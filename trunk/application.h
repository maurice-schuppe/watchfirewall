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
#include <sys/vnode.h>
#include <libkern/crypto/sha1.h>

#include "simpleBase.h"

class __attribute__((visibility("hidden"))) Application: public SimpleBase
{
public:
	static Application *applications;
	static IOLock *lock;
	static IOLock *lockRoutine;
	static IOThread thread;
	static SInt32 closing;
	static SInt32 countProcessesToCheck;
	
	static kauth_listener_t processListener;

public:
	static bool initStatic();
	static void freeStatic();
	static void checkIsLiveRoutine(void *arg);
	static Application* getApplication(); 
	static Application* addApplication(vfs_context_t vfsContext, vnode_t vnode);
	static Application* addApplication(kauth_cred_t cred, vnode_t vnode, const char *filePath);
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
	static int callbackVnodeListener
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
	gid_t gid;
	OSString *processName;
	OSString *filePath;
	
	Application *prev;
	Application *next;

	virtual void free()
	{
		::IOLog("application deleted pid: %d\n", this->pid);
		
		if(filePath)
			filePath->release();
		
		if(processName)
			processName->release();
		
		SimpleBase::free();
	}
	
	Application* removeFromChain()
	{
		if(prev)
			prev->next = next;
		
		if(this == applications)
			applications = next;
		
		if(next)
			next->prev = prev;
		
		prev = next = NULL;
		return this;
	}
	
};


#endif 