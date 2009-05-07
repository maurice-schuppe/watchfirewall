
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

#include "memory.h"
#include "protocol.h"
#include "firewall.h"
#include "cookie.h"



#define PRINT_MESSAGES
#define COOKIE_CONST 1234567

//debug message counter

signed long message_counter;

SInt32 attached_sokets;



#pragma mark Firewall

#define FIREWALL_SOKET_STATE_ALLOWED_BY_RULE 1
#define FIREWALL_SOKET_STATE_ALLOWED_FIREWALL_DOWN 2
#define FIREWALL_SOKET_STATE_DISABLED 3
#define FIREWALL_SOKET_STATE_WAIT_RULE 4


#pragma mark mutex

static lck_mtx_t		*gmutex = NULL;				// used to protect the tl_active and the tl_done queues
static lck_grp_t		*gmutex_grp = NULL;

#pragma mark clinet connection node

typedef struct _watch_client_connection_node
{
	struct _watch_client_connection_node *next;
	u_int32_t unit;
}watch_client_connection_node_t, *watch_client_connection_node;

watch_client_connection_node watch_root_client_connetions;

errno_t
watch_add_client_connection_node(u_int32_t unit, void **unitinfo)
{
	watch_client_connection_node node = (watch_client_connection_node)OSMalloc(sizeof(watch_client_connection_node_t), oSMallocTag);
	
	if(node == NULL)
	{
		return ENOMEM;
	}
	
	node->next = watch_root_client_connetions;
	node->unit = unit;
	
	watch_root_client_connetions = node;
	*unitinfo = node;
	
	return 0;
}

void 
watch_remove_client_connection_node(u_int32_t unit)
{
	watch_client_connection_node prev = NULL;
	watch_client_connection_node curr = watch_root_client_connetions;
	
	while(curr)
	{
		if(curr->unit == unit)
		{
			if(prev)
				prev->next = curr->next;
			else
				watch_root_client_connetions = curr->next;
			
			OSFree(curr, sizeof(watch_client_connection_node_t), oSMallocTag);
			break;
		}
		
		prev = curr;
		curr = curr->next;
	}	
}

#pragma mark client connections

kern_ctl_ref
watch_kernel_control_reference;

errno_t 
watch_kernel_control_send_to_all(void* data, size_t length)
{
	lck_mtx_lock(gmutex);
	
	if(!watch_kernel_control_reference)
		return KERN_SUCCESS;
	
	watch_client_connection_node node = watch_root_client_connetions;
		
	while(node)
	{
		size_t space = 0;
		do
		{
			if(!ctl_getenqueuespace(watch_kernel_control_reference, node->unit, &space))
			{
				if(space < length)
					IOSleep(50);
			}
			else
			{
				printf("ERROR get enqueue space");
				goto next;
			}
			
		}while(space < length);
			
		if(ctl_enqueuedata(watch_kernel_control_reference, node->unit, data, length, 0))
			printf("ERROR sending data to %d %s\n", node->unit, data);
//		else
//			printf("sended data(size-> %d) to %d %s\n", length, node->unit, data);
		
	next:;
		node = node->next;
	}
	
	lck_mtx_unlock(gmutex);
	
	return KERN_SUCCESS;
}

errno_t 
watch_kernel_control_connect(kern_ctl_ref kctlref, struct sockaddr_ctl *sac, void **unitinfo)
{
	//#ifdef PRINT_MESSAGES
	printf("ctl_client_register id 0x%x, ref 0x%x \n", sac->sc_unit, kctlref);	
	//#endif	
	return watch_add_client_connection_node(sac->sc_unit, unitinfo);	
	
}

errno_t 
watch_kernel_control_disconnect(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo)
{
	//#ifdef PRINT_MESSAGES
	printf("ctl_client_unregister id 0x%x \n", unit);	
	//#endif	
	
	
	
	//static name_proc[] = "kern.proc.pid.     ";
//	static struct kinfo_proc watch_proc;
//	
//	size_t len = sizeof(struct kinfo_proc);
//	watch_proc.kp_proc.p_comm[0] = '\0';
//	watch_proc.kp_proc.p_textvp = NULL;
//	
//	//snprintf(<#char * #>, <#size_t #>, <#const char * #>)
//	
//	if (sysctlbyname("kern.proc.pid.242", &watch_proc, &len, NULL, 0) != -1)
//	{
//		static char name_buffer[1001];
//		int len1 = 1000;
//		name_buffer[0] = '\0';
//		
//		printf("client exutable vnode : 0x%x", watch_proc.kp_proc.p_textvp);
//		
//		vn_getpath (watch_proc.kp_proc.p_textvp, name_buffer, &len1);// proc.kp_proc.p_textvp
//		
//		printf("client executable is : %s\n", name_buffer);
//		
//	}
//	else
//	{		
//		printf("error: get exutable name  \n");
//	}
	
	watch_remove_client_connection_node(unit);
	return KERN_SUCCESS;
}

errno_t 
watch_kernel_control_send(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, mbuf_t m, int flags)
{	
	return KERN_SUCCESS;
}

errno_t 
watch_kernel_control_set_socket_option(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t len)
{
	return KERN_SUCCESS;
}

errno_t 
watch_kernel_control_get_socket_option(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t *len)
{
	return KERN_SUCCESS;
}

errno_t
watch_send_text_message_sokect_info_opt(char *operation_name, socket_t so, void* cookie, int get_socket_info)
{
	signed long i = message_counter;
	OSIncrementAtomic(&message_counter);
	
	if(oSMallocTag == NULL)
		return ENOMEM;
	
	char* text_buffer = (char*)OSMalloc(1000, oSMallocTag);
	if(text_buffer == NULL)
		return ENOMEM;
	
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
		 if(*(uint32_t*)cookie == COOKIE_CONST)
			 cookieState = ((char*)cookie)[5];
		else
			cookieState = -1;
	}		
	
	// "%-10d operation: %-20s; user_id: %-4d, proc_id: %-4d; proc_name: %-20s; socket 0x%-10x; domain: %-3d; type: %-3d; protocol: %-3d \n"
	
	snprintf(text_buffer, 1000,
			 "%-10d; %-4d; %-10d ; %-20s; %-4d; %-4d; %-20s; %-4d; %-4d; %-4d \n", 
			 so,
			 cookieState,
			 i,
			 operation_name,
			 user_id,
			 proc_id,
			 proc_name,			  
			 domain, 
			 type, 
			 protocol );


	watch_kernel_control_send_to_all(text_buffer, strlen(text_buffer) + 1);
	
	OSFree(text_buffer, 1000, oSMallocTag);
	return KERN_SUCCESS;
}

errno_t
watch_send_text_message(char *operation_name, socket_t so, void* cookie)
{
	return watch_send_text_message_sokect_info_opt(operation_name, so, cookie, 1);
}

struct kern_ctl_reg 
watch_kernel_control_registration = 
{
MYBUNDLEID,													/* use a reverse dns name which includes a name unique to your comany */
0,															/* set to 0 for dynamically assigned control ID - CTL_FLAG_REG_ID_UNIT not set */
0,															/* ctl_unit - ignored when CTL_FLAG_REG_ID_UNIT not set */
0,//CTL_FLAG_PRIVILEGED, //removed for test										/* privileged access required to access this filter */
(8 * 1024),													/* Override receive buffer size */
(8 * 1024),													/* Override receive buffer size */
watch_kernel_control_connect,								/* Called when a connection request is accepted */
watch_kernel_control_disconnect,							/* called when a connection becomes disconnected */
watch_kernel_control_send,									/* ctl_send_func - handles data sent from the client to kernel control  */
watch_kernel_control_set_socket_option,						/* called when the user process makes the setsockopt call */
watch_kernel_control_get_socket_option						/* called when the user process makes the getsockopt call */
};

#pragma mark client protocol

void
watch_kernel_control_sended(watch_client_connection_node client_node, mbuf_t m, int flags)
{
	size_t mlen = mbuf_len(m);
	printf("in kernel control send mbuf len is: %d \n", mlen);
	printf("in kernel control send data is : %s", mbuf_data(m));
}

#pragma mark firewall rules

#pragma mark applications



#pragma mark nke
typedef struct _protos
{
	sflt_handle handle;
	int domain;
	int type;
	int protocol;
	int state;
}protos_t, *protos;

protos_t protocols[] =
{
	{0xBACAF000, PF_INET, SOCK_STREAM, IPPROTO_TCP, 0},
	{0xBACAF001, PF_INET, SOCK_DGRAM, IPPROTO_UDP, 0},
	{0xBACAF002, PF_INET, SOCK_RAW, IPPROTO_ICMP, 0},
	{0xBACAF003, PF_INET6, SOCK_STREAM, IPPROTO_TCP, 0},
	{0xBACAF004, PF_INET6, SOCK_DGRAM, IPPROTO_UDP, 0},
	{0xBACAF005, PF_INET6, SOCK_RAW, IPPROTO_ICMP, 0}//,
	//{0xBACAF006, PF_UNIX, SOCK_STREAM, 0, 0}

};

void	
watch_unregistered_func(sflt_handle handle)
{
	
}


errno_t	
watch_attach_func(void	**cookie, socket_t so)
{
	
	char *ch = (char*)OSMalloc(30, oSMallocTag);
	if(ch == NULL)
	{
		printf("error allocation memory");
	}
	else
	{
		*(uint32_t*)ch = COOKIE_CONST;
		ch[5] = 1;
		*cookie = ch;
		//watch_send_text_message("attach", so, *cookie);
		OSIncrementAtomic(&attached_sokets);
	}
	
	
	return 0;
}

void	
watch_detach_func(void	*cookie, socket_t so)
{
	watch_send_text_message_sokect_info_opt((char*)"detach", so, cookie, 0);
	if(cookie)
	{
		OSDecrementAtomic(&attached_sokets);
		OSFree(cookie, 30, oSMallocTag);
	}
}

void	
watch_notify_func(void *cookie, socket_t so, sflt_event_t event, void *param)
{
//	OSIncrementAtomic8(cookie + 5);
//	watch_send_text_message("notify", so, cookie);
}

int		
watch_getpeername_func(void *cookie, socket_t so, struct sockaddr **sa)
{
//	OSIncrementAtomic8(cookie + 5);
//	watch_send_text_message("getpeername", so, cookie);
	return 0;
}


int		
watch_getsockname_func(void *cookie, socket_t so, struct sockaddr **sa)
{
//	OSIncrementAtomic8(cookie + 5);
//	watch_send_text_message("getsockname", so, cookie);
	return 0;
}


errno_t	
watch_data_in_func(void *cookie, socket_t so, const struct sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{
	OSIncrementAtomic8((SInt8*)cookie + 5);
	//watch_send_text_message("data in", so, cookie);
	return 0;
	
	//check fo socket changes
	SocketCookie *scookie = (SocketCookie*)cookie;
	
	if(Firewall::instance->IsChanged(0))
	{
		//update rule and set in cookie
		Rule* rule = Firewall::instance->FindRule( 
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


errno_t	
watch_data_out_func(void *cookie, socket_t so, const struct sockaddr *to, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{
	OSIncrementAtomic8((SInt8*)cookie + 5);
	//watch_send_text_message("data out", so, cookie);
	return 0;
}


errno_t	
watch_connect_in_func(void *cookie, socket_t so, const struct sockaddr *from)
{
	OSIncrementAtomic8((SInt8*)cookie + 5);
	//watch_send_text_message("connect in", so, cookie);
	return 0;
	
	//check is firewall up
	
	//check is have rule
		//if have apply
		//else allow and ask
	
	//check is monitor up
}


errno_t	
watch_connect_out_func(void *cookie, socket_t so, const struct sockaddr *to)
{
	OSIncrementAtomic8((SInt8*)cookie + 5);
	//watch_send_text_message("connect out", so, cookie);
	return 0;
}


errno_t	
watch_bind_func(void *cookie, socket_t so, const struct sockaddr *to)
{
//	OSIncrementAtomic8(cookie + 5);
//	watch_send_text_message("bind", so, cookie);
	return 0;
	
	//check is allowed to bind to that address
}


errno_t	
watch_setoption_func(void *cookie, socket_t so, sockopt_t opt)
{
	//OSIncrementAtomic8(cookie + 5);
	//watch_send_text_message("set option", so, cookie);
	return 0;
}

errno_t	
watch_getoption_func(void *cookie, socket_t so, sockopt_t opt)
{
//	OSIncrementAtomic8(cookie + 5);
//	watch_send_text_message("get optin", so, cookie);
	return 0;
}

errno_t	
watch_listen_func(void *cookie, socket_t so)
{
//	OSIncrementAtomic8(cookie + 5);
//	watch_send_text_message("listen", so, cookie);
	return 0;
	
	//that is for tcp //check is alowed listen
	//if not rule allow and process in accept function
}

errno_t	
watch_ioctl_func(void *cookie, socket_t so, u_int32_t request, const char* argp)
{
//	OSIncrementAtomic8(cookie + 5);
//	watch_send_text_message("ioctl", so, cookie);
	return 0;
}

errno_t 
watch_accept_func(void *cookie, socket_t so_listen, socket_t so, const struct sockaddr *local, const struct sockaddr *remote)
{
//	OSIncrementAtomic8(cookie + 5);
//	watch_send_text_message("accept", so, cookie);
	return 0;
	
	//check if allowed from that remote address
}



#pragma mark tcplog Filter Definition

/* Dispatch vector for TCPLogger IPv4 socket functions */
static struct sflt_filter watch_sflt_filter = {
0xBABABABC,					/* sflt_handle - use a registered creator type - <http://developer.apple.com/datatype/> */
SFLT_GLOBAL | SFLT_EXTENDED,/* sf_flags */
(char*)MYBUNDLEID,					/* sf_name - cannot be nil else param err results */
watch_unregistered_func,	/* sf_unregistered_func */
watch_attach_func,			/* sf_attach_func - cannot be nil else param err results */			
watch_detach_func,			/* sf_detach_func - cannot be nil else param err results */
watch_notify_func,			/* sf_notify_func */
watch_getpeername_func,		/* sf_getpeername_func */
watch_getsockname_func,		/* sf_getsockname_func */
watch_data_in_func,			/* sf_data_in_func */
watch_data_out_func,		/* sf_data_out_func */
watch_connect_in_func,		/* sf_connect_in_func */
watch_connect_out_func,		/* sf_connect_out_func */
watch_bind_func,			/* sf_bind_func */
watch_setoption_func,		/* sf_setoption_func */
watch_getoption_func,		/* sf_getoption_func */
watch_listen_func,			/* sf_listen_func */
watch_ioctl_func,			/* sf_ioctl_func */
{sizeof(sflt_filter::sflt_filter_ext), watch_accept_func, {NULL,NULL,NULL,NULL,NULL}} /*sf_filter_ext */
};



#pragma mark kext routine

extern "C" {

kern_return_t 
watch_start (kmod_info_t * ki, void * d)
{	
	
	printf("watch kext start\n");	
	
	Firewall::initInstance();
	
	oSMallocTag = OSMalloc_Tagalloc("com.janbird.watch.kext", OSMT_DEFAULT);
	
	//init mutex
	gmutex_grp = lck_grp_alloc_init(MYBUNDLEID, LCK_GRP_ATTR_NULL);
	if (gmutex_grp != NULL)	
	{
		gmutex = lck_mtx_alloc_init(gmutex_grp, LCK_ATTR_NULL);
	}	
	
	
	
	size_t len_protocols = sizeof(protocols)/sizeof(*protocols);
	
	printf("len protocols is : %d \n", len_protocols);
	
	for(int k =0 ; k < len_protocols; k++)
	{
		watch_sflt_filter.sf_handle = protocols[k].handle;
		errno_t retval = sflt_register(&watch_sflt_filter, protocols[k].domain, protocols[k].type, protocols[k].protocol);
		printf("sflt_register returned result %d for ip4 filter.\n", retval);
		if(!retval)
			protocols[k].state = 1;
	}
	
	/* Register the NKE */
	// register the filter with AF_INET domain, SOCK_STREAM type, TCP protocol and set the global flag
//	errno_t retval = sflt_register(&watch_sflt_filter_ip4, PF_INET, SOCK_STREAM, IPPROTO_TCP);
//	printf("sflt_register returned result %d for ip4 filter.\n", retval);
//	if (retval != 0)
//		return KERN_FAILURE;
	
	
	
	 errno_t retval = ctl_register(&watch_kernel_control_registration, &watch_kernel_control_reference);
#ifdef PRINT_MESSAGES
	if (retval == 0)
		printf("ctl_register id 0x%x, ref 0x%x \n", watch_kernel_control_registration.ctl_id, watch_kernel_control_reference);	
	else	
		printf("ctl_register returned error %d\n", retval);		
#endif
	
		
	
	    return KERN_SUCCESS;
}


kern_return_t 
watch_stop (kmod_info_t * ki, void * d) 
{
	
	printf("watch kext stop\n");	
	
	
	size_t len_protocols = sizeof(protocols)/sizeof(*protocols);
	for(int k =0 ; k < len_protocols; k++)
	{
		if(protocols[k].state)	
			if(!sflt_unregister(protocols[k].handle))
				protocols[k].state = 0;
	}
	
	if(attached_sokets > 0)
		return EBUSY;
	
	watch_kernel_control_send_to_all((void*)"exit", 5);
	
	//lck_mtx_lock(gmutex);
	
	if(ctl_deregister(watch_kernel_control_reference))	
	{
		printf ("kernel contorl return error");
		//lck_mtx_unlock(gmutex);
		return EBUSY;
	}	
	
	watch_kernel_control_reference = NULL;
	//lck_mtx_unlock(gmutex);
	
	
	
	if (gmutex_grp)
	{
		//free mutex
		if (gmutex)
		{
			lck_mtx_free(gmutex, gmutex_grp);
			gmutex = NULL;
		}
		
		
		lck_grp_free(gmutex_grp);
		gmutex_grp = NULL;
	}
	
	//end free mutex	
	
	
	if(oSMallocTag)
	{
		OSMalloc_Tagfree(oSMallocTag);
		oSMallocTag = NULL;
	}

	
    return KERN_SUCCESS;
}

}