
#define SYSCTL_DEF_ENABLED
#define PROC_DEF_ENABLED

#include <mach/vm_types.h>
#include <mach/kmod.h>
#include <sys/socket.h>
#include <sys/kpi_socket.h>
#include <sys/kpi_mbuf.h>
#include <sys/kpi_socket.h>
#include <sys/kpi_socketfilter.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/mbuf.h>
#include <netinet/in.h>
#include <kern/locks.h>
#include <kern/assert.h>
#include <kern/debug.h>

#include <libkern/OSMalloc.h>
#include <libkern/OSAtomic.h>
#include <sys/kern_control.h>
#include <sys/kauth.h>
#include <sys/time.h>
#include <stdarg.h>

#include <IOKit/IOLib.h>

#include "firewall.h"
#include "cookie.h"

errno_t
watch_send_text_message_sokect_info_opt(char *operation_name, socket_t so, void* cookie, int get_socket_info)
{
	
	
	char* text_buffer = NULL;
	
	int domain = -1;
	int type = -1;
	int protocol = -1;
	if(so && get_socket_info)
		sock_gettype(so, &domain, &type, &protocol);
	
	char proc_name[100];
	proc_name[0] = 0;
	proc_selfname(proc_name, 100);
	
	int user_id = kauth_getuid();
	int proc_id = proc_selfpid();
	
	int cookieState = 0;
	if(cookie)
	{
		 if(*(uint32_t*)cookie == 0)
			 cookieState = ((char*)cookie)[5];
		else
			cookieState = -1;
	}		
	
	// "%-10d operation: %-20s; user_id: %-4d, proc_id: %-4d; proc_name: %-20s; socket 0x%-10x; domain: %-3d; type: %-3d; protocol: %-3d \n"
	
	snprintf(text_buffer, 1000,
			 "%-10d; %-4d; %-10d ; %-20s; %-4d; %-4d; %-20s; %-4d; %-4d; %-4d \n", 
			 so,
			 cookieState,
			 0,
			 operation_name,
			 user_id,
			 proc_id,
			 proc_name,			  
			 domain, 
			 type, 
			 protocol );

	return KERN_SUCCESS;
}

errno_t	
watch_data_in_func(void *cookie, socket_t so, const struct sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{
	OSIncrementAtomic8((SInt8*)cookie + 5);
	//watch_send_text_message("data in", so, cookie);
	return 0;
	
	//check fo socket changes
	SocketCookie *scookie = (SocketCookie*)cookie;
	
	if(Firewall::instance->isChanged(0))
	{
		//update rule and set in cookie
		Rule* rule = Firewall::instance->findRule( 
													NULL, NULL, 
													0, 0, 0, 
													0, 
													NULL);
		if(rule == NULL)
		{
			scookie->state == CookieStateNOT;
		}
		
	}
	
	switch (scookie->state) 
	{
		case CookieStateNOT:
			//send ask
			scookie->state = CookieStateASK;
			//scookie->aks_rule_time = time();//TODO: insert header
		
		case CookieStateASK:
			//cashe requiest
			return EJUSTRETURN;
		case CookieStateALLOWED:
			return 0;
		case CookieStateNOT_ALLOWED:
			return -1;//fix return value
		default:
			break;
	}
	
	return -1;
	
}

#pragma mark kext routine

extern "C" {

kern_return_t 
watch_start (kmod_info_t * ki, void * d)
{		
	if(!Firewall::Open())
		return KERN_FAILURE;

	return KERN_SUCCESS;
}


kern_return_t 
watch_stop (kmod_info_t * ki, void * d) 
{
	IOLog("in watch_stop \n");
	if(!Firewall::Close())
		return EBUSY;
	
	IOLog("stoped close return true \n");
	return KERN_SUCCESS;
}

}