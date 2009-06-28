#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/kauth.h>
#include "application.h"

Application			*Application::applications;
IOLock				*Application::lock;
IOLock				*Application::lockRoutine;
kauth_listener_t	Application::processListener;
IOThread			Application::thread;
SInt32				Application::closing;
SInt32				Application::countProcessesToCheck;


bool 
Application::initStatic()
{
	closing = 0;
	
	if(lock = IOLockAlloc())
	{
		if(lockRoutine = IOLockAlloc())
		{
			if(thread = IOCreateThread(checkIsLiveRoutine, NULL))
			{
				
				if(processListener = kauth_listen_scope(KAUTH_SCOPE_FILEOP, callbackProcessListener, NULL))
				{
					return true;
				}
				
				OSIncrementAtomic(&closing);
			}
			
			IOLockFree(lockRoutine);
		}
		
		IOLockFree(lock);
	}
	
	return false;
}


void 
Application::freeStatic()
{
	OSIncrementAtomic(&closing);
	
	if(processListener)
		kauth_unlisten_scope(processListener);
	
	processListener = NULL;
	
	if(lock)
	{
		if(lockRoutine)
		{
			//IOLockWakeup(lockRoutine, 0, false);
			//IOSleep(1);
			::IOLog("begin waiting for application routine ended\n");
			IOLockLock(lockRoutine);
			::IOLog("application routine ended...\n");
			IOLockUnlock(lockRoutine);
			IOLockFree(lockRoutine);
			lockRoutine = NULL;
		}
		
		IOLockLock(lock);
		while (applications)
			applications->removeFromChain()->release();
		IOLockUnlock(lock);
		
		IOLockFree(lock);
		
		lock = NULL;
	}
}


Application* 
Application::getApplication()
{
	if(Application::closing)
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
	if(result)
		result->retain();
	
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
	result->retain();
	
	::IOLog("application added pid: %d\n", result->pid);
	
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
	if(KAUTH_FILEOP_EXEC == action && Application::closing == 0)
	{
		IOLockLock(Application::lock);
		Application::addApplication((vnode_t)arg0, (const char *) arg1);
		countProcessesToCheck++;
		IOLockUnlock(Application::lock);
	}
	
	return KAUTH_RESULT_DEFER;
}


void
Application::checkIsLiveRoutine(void *arg)
{
	Application *app = NULL;
	
	IOLockLock(lockRoutine);
	
	while(closing == 0)
	{
		if(countProcessesToCheck == 0)
			IOSleep(500);
		
		if(closing)
			break;
	
		IOLockLock(Application::lock);
		if(app == NULL)
			app = Application::applications;
		else
		{
			Application* a = app;
			app = app->next;
			a->release();
		}
		
		if(countProcessesToCheck > 0)
			countProcessesToCheck--;
		
		if(app)
		{	
			proc_t pr = proc_find(app->pid);
			if(pr)
			{
				proc_rele(pr);
				app->retain();//cashing for next loop
			}
			else
			{
				::IOLog("app routine delete: %d ref: %d\n", app->pid, app->references);
				app->removeFromChain();
				app->release();
				app = NULL;
				countProcessesToCheck++;
			}
		}
		IOLockUnlock(Application::lock);
	}	

	::IOLog("app routine exit\n");
	IOLockUnlock(lockRoutine);
	IOExitThread();
}


