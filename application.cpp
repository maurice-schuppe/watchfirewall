#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/kauth.h>
#include "application.h"

Application			*Application::applications;
IOLock				*Application::lock;
kauth_listener_t	Application::processListener;
IOThread			Application::thread;
volatile SInt32		Application::closing;


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
		IOLockUnlock(Application::lock);
	}
	
	return KAUTH_RESULT_DEFER;
}

void
Application::checkIsLiveRoutine(void *arg)
{
	Application *app = NULL;
	while(closing == 0)
	{
		IOSleep(100);
		if(closing)
			break;
	
		//::IOLog("new app routine\n");
		IOLockLock(Application::lock);
		if(app == NULL)
			app = Application::applications;
		else
		{
			Application* a = app;
			app = app->next;
			a->release();
		}
		
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
			}
		}
		IOLockUnlock(Application::lock);
	}	

	::IOLog("app routine exit\n");
	IOExitThread();
}


