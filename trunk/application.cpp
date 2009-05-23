/*
 *  application.cpp
 *  Watch
 *
 *  Created by JanBird on 5/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <sys/proc.h>
#include <sys/kauth.h>
#include "application.h"

Application *Application::applications;
IOLock *Application::lock;


Application* 
Application::getApplication()
{
	if(!lock)
		return NULL;
	
	//search in existing
	pid_t pid = proc_selfpid();
	IOLockLock(Application::lock);	
	Application* result = applications;
	char procName[255] = {0};
	
	while(result)
	{
		if(result->pid == pid)
		{
			result->retain();
			::IOLog("application return existing result. pid: %d\n", result->pid);
			goto unlock;
		}
		
		result = result->next;
	}
	
	//colect info
	result = new Application();
	if(!result)
		goto unlock;
	
	proc_selfname(procName, 254);
	result->processName = OSString::withCString(procName);
	result->processPath = NULL;//TODO: refactor
	
	result->uid = kauth_getuid();
	result->pid = pid;
	result->p_pid = proc_selfppid();
	result->next = applications;
	applications = result;
	
	if(result->next)
		result->next->prev = result;
	
	result->retain();
	::IOLog("application return new result. pid: %d\n", result->pid);

unlock:
	
	IOLockUnlock(Application::lock);
	return result;	
}
