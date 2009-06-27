#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/kauth.h>
#include "application.h"

Application *Application::applications;
IOLock *Application::lock;
kauth_listener_t Application::processListener;



Application* 
Application::getApplication()
{
	if(!lock)
		return NULL;
	
	//search in existing
	pid_t pid = proc_selfpid();
	IOLockLock(Application::lock);	
	Application* result = applications;
	
	while(result)
	{
		if(result->pid == pid)
		{
			result->retain();
			IOLockUnlock(Application::lock);
			return result;	
		}
		
		result = result->next;
	}
	
	result = addApplication(NULL, NULL);
	result->retain();

unlock:
	
	IOLockUnlock(Application::lock);
	return result;	
}


Application* 
Application::addApplication(vnode_t vnode, const char* filePath)
{
	char procName[MAXCOMLEN+1] = {0};
	
	Application *result = new Application();
	if(!result)
		return NULL;
	
	proc_selfname(procName, MAXCOMLEN+1);
	
	result->filePath = OSString::withCString(filePath);
	result->processName = OSString::withCString(procName);
	
	if(vnode)
	{
		//collect additional file info
	}
	
	result->uid = kauth_getuid();
	result->pid = proc_selfpid();
	result->p_pid = proc_selfppid();
	result->next = applications;
	applications = result;
	
	if(result->next)
		result->next->prev = result;
	
	return result;
}


int 
Application::callbackProcessListener
(
 kauth_cred_t    credential,
 void *          idata,
 kauth_action_t  action,
 uintptr_t       arg0,
 uintptr_t       arg1,
 uintptr_t       arg2,
 uintptr_t       arg3
 )
{
	if(KAUTH_FILEOP_EXEC == action)
	{
		printf(
			   "scope=" KAUTH_SCOPE_FILEOP ", action=KAUTH_FILEOP_EXEC, uid=%ld, pid=%ld, vnode=0x%lx, path=%s\n", 
			   (long) kauth_cred_getuid(credential),
			   (long) proc_selfpid(),
			   (long) arg0,
			   (const char *) arg1
			   );
	}
	
	return KAUTH_RESULT_DEFER;
}

