#include <sys/proc.h>
#include <sys/fcntl.h>
#include <sys/vnode_if.h>
#include "application.h"

Application			*Application::applications;
IOLock				*Application::lock;
IOLock				*Application::lockRoutine;
kauth_listener_t	Application::processListener;
IOThread			Application::thread;
SInt32				Application::closing;
SInt32				Application::countProcessesToCheck;

void
hexdump(const unsigned char *sha1, char* buffer)
{
	static const char hexval[16] = { '0', '1', '2', '3','4', '5', '6', '7','8', '9', 'A', 'B','C', 'D', 'E', 'F' };
	int k = 0;
	for (int i = 0 ; i < SHA1_RESULTLEN; i++)
	{
		buffer[k++] = hexval[ ( (sha1[i] >> 4) & 0xF ) ];
		buffer[k++] = hexval[ ( sha1[i] & 0x0F ) ];
	}
	buffer[k] = 0;
}

char testText[] = "MMMMMMMMMMM";

void 
testOpenFile()
{
	static bool ex = true;
	
	if(ex)
	{
		vnode_t vp;
		vfs_context_t vfs = vfs_context_current();
		if (vnode_open("/file.txt", (O_CREAT | O_TRUNC | FWRITE), (0), 0, &vp, vfs) == 0)
		{
			
			uio_t auio;
			auio = uio_create(1, 0/*offset*/, UIO_SYSSPACE, UIO_WRITE);
			uio_addiov(auio, (user_addr_t)testText, sizeof(testText));
			
			VNOP_WRITE(vp, auio, /*ioflg*/ 0, vfs);
			
			::IOLog("file open \n");
			vnode_close(vp, 0, vfs);
		}
		else
		{
			::IOLog("file can't open \n");
		}

		ex = false;
	}
}

bool 
Application::initStatic()
{
	closing = 0;
	
	testOpenFile();
	
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
			IOLockLock(lockRoutine);
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
	
	result = addApplication(NULL, NULL, NULL);
	if(result)
		result->retain();
	
	IOLockUnlock(Application::lock);
	return result;	
}


Application* 
Application::addApplication(vfs_context_t vfsContext, vnode_t vnode)
{
	char procName[MAXCOMLEN] = {0};
	vnode_attr vnodeAttr;
	bzero(&vnodeAttr, sizeof(vnode_attr)); 
	
	Application *result = new Application();
	if(!result)
		return NULL;
	
	
	if(vfsContext && vnode && (vnode_getattr(vnode, &vnodeAttr, vfsContext) == 0))
	{
		//test
		//unsigned char cdHash[SHA1_RESULTLEN];
		char textCdHash[SHA1_RESULTLEN * 2 + 1] = {0};
		//vn_getcdhash(vnode, 0, cdHash);
		//hexdump(cdHash, textCdHash);
		
		
		char filePath[1024] = {0};//in memory not in stack..
		int size = 1024;
		vn_getpath(vnode, filePath, &size);
		result->filePath = OSString::withCString(filePath);

		result->uid = kauth_cred_getuid(vfs_context_ucred(vfsContext));
		
		proc_t proc = vfs_context_proc(vfsContext);
		result->pid = proc_pid(proc);
		result->p_pid = proc_ppid(proc);
		
		proc_name(result->pid, procName, MAXCOMLEN);
		result->processName = OSString::withCString(procName);
		//??release proc

		::IOLog("pid: %d pidByContext: %d  cdhask: %s \n", proc_selfpid(), result->pid, textCdHash);

	}
	else
	{
		proc_selfname(procName, MAXCOMLEN);
		result->processName = OSString::withCString(procName);
		result->filePath = OSString::withCString("");
		result->uid = kauth_getuid();
		result->pid = proc_selfpid();
		result->p_pid = proc_selfppid();
	}
	
	result->retain();
	
	::IOLog("application added pid: %d path: %s\n", result->pid, result->filePath->getCStringNoCopy());
	
	result->next = applications;
	applications = result;
	
	if(result->next)
		result->next->prev = result;
	
	return result;
}


Application* 
Application::addApplication(kauth_cred_t cred, vnode_t vnode, const char *filePath)
{
	char procName[MAXCOMLEN] = {0};
	vnode_attr vnodeAttr;
	bzero(&vnodeAttr, sizeof(vnode_attr)); 
	
	Application *result = new Application();
	if(!result)
		return NULL;
	
	result->pid = proc_selfpid();
	result->p_pid = proc_selfppid();
	
	proc_selfname(procName, MAXCOMLEN);
	result->processName = OSString::withCString(procName);
	
	if(cred && vnode)
	{
		result->filePath = OSString::withCString(filePath);
		result->uid = kauth_cred_getuid(cred);
		result->gid = kauth_cred_getgid(cred);
	}
	else
	{
		result->filePath = OSString::withCString("");
		result->uid = kauth_getuid();
		result->gid = kauth_getgid();
	}
	
	result->retain();
	
	::IOLog("application added pid: %d path: %s\n", result->pid, result->filePath->getCStringNoCopy());
	
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
		Application::addApplication(credential, (vnode_t)arg0, (const char *) arg1);
		countProcessesToCheck++;
		IOLockUnlock(Application::lock);
	}
	
	return KAUTH_RESULT_DEFER;
}

int 
Application::callbackVnodeListener
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
	if(Application::closing == 0 && action & KAUTH_VNODE_EXECUTE)
	{
		if(vnode_isdir((vnode_t) arg1) == false)
		{
			IOLockLock(Application::lock);
			Application::addApplication((vfs_context_t)arg0, (vnode_t) arg1);
			countProcessesToCheck++;
			IOLockUnlock(Application::lock);
		}
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
				//::IOLog("app routine delete: %d ref: %d\n", app->pid, app->references);
				app->removeFromChain();
				app->release();
				app = NULL;
				countProcessesToCheck++;
			}
		}
		IOLockUnlock(Application::lock);
	}	

	//::IOLog("app routine exit\n");
	IOLockUnlock(lockRoutine);
	IOExitThread();
}

