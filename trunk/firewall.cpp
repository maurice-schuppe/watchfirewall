/*
 *  firewall.c
 *  Watch
 *
 *  Created by Jan Bird on 4/5/09.
 *  Copyright 2009 __MoonLight__. All rights reserved.
 *
 */

#include "firewall.h"


OSDefineMetaClassAndStructors(Firewall, OSObject)


Firewall *Firewall::instance = NULL;



protocol Firewall::protocols[] =
{
{0xBACAF000, PF_INET, SOCK_STREAM, IPPROTO_TCP, 0},
{0xBACAF001, PF_INET, SOCK_DGRAM, IPPROTO_UDP, 0},
{0xBACAF002, PF_INET, SOCK_RAW, IPPROTO_ICMP, 0},
{0xBACAF003, PF_INET6, SOCK_STREAM, IPPROTO_TCP, 0},
{0xBACAF004, PF_INET6, SOCK_DGRAM, IPPROTO_UDP, 0},
{0xBACAF005, PF_INET6, SOCK_RAW, IPPROTO_ICMP, 0}//,
//{0xBACAF006, PF_UNIX, SOCK_STREAM, 0, 0}

};

sflt_filter Firewall::sfltFilter = {
0xBABABABC,					/* sflt_handle - use a registered creator type - <http://developer.apple.com/datatype/> */
SFLT_GLOBAL | SFLT_EXTENDED,/* sf_flags */
(char*)MYBUNDLEID,					/* sf_name - cannot be nil else param err results */
unregistered,	/* sf_unregistered_func */
attach,			/* sf_attach_func - cannot be nil else param err results */			
detach,			/* sf_detach_func - cannot be nil else param err results */
notify,			/* sf_notify_func */
getpeername,		/* sf_getpeername_func */
getsockname,		/* sf_getsockname_func */
dataIn,			/* sf_data_in_func */
dataOut,		/* sf_data_out_func */
connectIn,		/* sf_connect_in_func */
connectOut,		/* sf_connect_out_func */
bind,			/* sf_bind_func */
setoption,		/* sf_setoption_func */
getoption,		/* sf_getoption_func */
listen,			/* sf_listen_func */
ioctl,			/* sf_ioctl_func */
{sizeof(sflt_filter::sflt_filter_ext), accept, {NULL,NULL,NULL,NULL,NULL}} /*sf_filter_ext */
};

bool 
Firewall::registerSocketFilters()
{
	size_t len_protocols = sizeof(protocols)/sizeof(*protocols);
	
	for(int k =0 ; k < len_protocols; k++)
	{
		sfltFilter.sf_handle = protocols[k].handle;
		errno_t retval = sflt_register(&sfltFilter, protocols[k].domain, protocols[k].type, protocols[k].protocol);
		if(!retval)
			protocols[k].state = 1;
	}
	
	return true;
}

bool 
Firewall::unregisterSocketFilters()
{
	size_t len_protocols = sizeof(protocols)/sizeof(*protocols);
	for(int k =0 ; k < len_protocols; k++)
	{
		if(protocols[k].state)	
			if(!sflt_unregister(protocols[k].handle))
				protocols[k].state = 0;
	}
	
	return true;
}



#pragma mark socket filter functions
void	
Firewall::unregistered(sflt_handle handle)
{
	
}


errno_t	
Firewall::attach(void **cookie, socket_t so)
{
	if(!Firewall::instance)
		return KERN_FAILURE;
	
	SocketCookie *socketCookie = new SocketCookie();
	
	if(socketCookie == NULL)
	{
		IOLog("can't allocate memory for scoket cookie");
		return KERN_MEMORY_ERROR;
	}

	*cookie = socketCookie;
	//add to known sockets
	
	return KERN_SUCCESS;
}

void	
Firewall::detach(void *cookie, socket_t so)
{
	Firewall::instance->send(NULL);
	if(cookie)
	{
		//remove from known sockets
		((SocketCookie*)cookie)->release();
	}
}

void	
Firewall::notify(void *cookie, socket_t so, sflt_event_t event, void *param)
{
	//	OSIncrementAtomic8(cookie + 5);
	//	watch_send_text_message("notify", so, cookie);
}

int		
Firewall::getpeername(void *cookie, socket_t so, struct sockaddr **sa)
{
	//	OSIncrementAtomic8(cookie + 5);
	//	watch_send_text_message("getpeername", so, cookie);
	return 0;
}


int		
Firewall::getsockname(void *cookie, socket_t so, struct sockaddr **sa)
{
	//	OSIncrementAtomic8(cookie + 5);
	//	watch_send_text_message("getsockname", so, cookie);
	return 0;
}


errno_t	
Firewall::dataIn(void *cookie, socket_t so, const struct sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{
	return KERN_SUCCESS;
	
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
			return KERN_SUCCESS;
		case CookieStateNOT_ALLOWED:
			return KERN_POLICY_LIMIT;//fix return value
		default:
			break;
	}
	
	return -1;
	
}


errno_t	
Firewall::dataOut(void *cookie, socket_t so, const struct sockaddr *to, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{
	return KERN_SUCCESS;
}


errno_t	
Firewall::connectIn(void *cookie, socket_t so, const struct sockaddr *from)
{
	return KERN_SUCCESS;
	
	//check is firewall up
	
	//check is have rule
	//if have apply
	//else allow and ask
	
	//check is monitor up
}


errno_t	
Firewall::connectOut(void *cookie, socket_t so, const struct sockaddr *to)
{
	return KERN_SUCCESS;
}


errno_t	
Firewall::bind(void *cookie, socket_t so, const struct sockaddr *to)
{
	return KERN_SUCCESS;
	
	//check is allowed to bind to that address
}


errno_t	
Firewall::setoption(void *cookie, socket_t so, sockopt_t opt)
{
	return KERN_SUCCESS;
}

errno_t	
Firewall::getoption(void *cookie, socket_t so, sockopt_t opt)
{
	return KERN_SUCCESS;
}

errno_t	
Firewall::listen(void *cookie, socket_t so)
{
	return KERN_SUCCESS;
	
	//that is for tcp //check is alowed listen
	//if not rule allow and process in accept function
}

errno_t	
Firewall::ioctl(void *cookie, socket_t so, u_int32_t request, const char* argp)
{
	return KERN_SUCCESS;
}

errno_t 
Firewall::accept(void *cookie, socket_t so_listen, socket_t so, const struct sockaddr *local, const struct sockaddr *remote)
{
	return KERN_SUCCESS;
	
	//check if allowed from that remote address
}




bool 
Firewall::Open()
{
	instance = new Firewall();
	
	if(!instance->init())
	{
		instance->release();
		return false;
	}
	
	if(instance->registerSocketFilters())
	{
		if(instance->registerKernelControl())
		{
			return true;
		}
		
		instance->unregisterSocketFilters();
	}
	
	instance->release();
	return false;
}

bool
Firewall::Close()
{
	if(instance == NULL)
		return true;
	
	if(!instance->unregisterSocketFilters())
		return false;
	
	//wait to close open sockets
	
	//check for connections
	if(!instance->unRegisterKernelControl())
		return false;
	
	return true;
}

bool 
Firewall::init()
{
	if(!OSObject::init())
		return false;
	
	
	return true;
}

bool 
Firewall::isChanged(time_t)
{
	//if(this->) check for last time changed rules
	return 1;//not changed
}

Rule* 
Firewall::findRule(const char* process_name, const char* process_path, 
									 UInt16 sock_famely, UInt16 socket_type, UInt16 sock_protocol, 
									 UInt8 direction, struct sockaddr *sockaddres )
{
	//TODO: refactor
	RuleNode *current_rule = rules;
	
	while(current_rule)
	{
		//if(current_rule->rule->)
	}
	
	return NULL;
}


Rule* 
Firewall::addRule(Rule *rule)
{
	return NULL;
}

Rule* 
Firewall::deleteRule(Rule *rule)
{
	return NULL;
}

#pragma mark kernel control

kern_ctl_reg 
Firewall::kernelControlRegistration = 
{
MYBUNDLEID,								/* use a reverse dns name which includes a name unique to your comany */
0,										/* set to 0 for dynamically assigned control ID - CTL_FLAG_REG_ID_UNIT not set */
0,										/* ctl_unit - ignored when CTL_FLAG_REG_ID_UNIT not set */
0,//CTL_FLAG_PRIVILEGED, //removed for test										/* privileged access required to access this filter */
(8 * 1024),								/* Override receive buffer size */
(8 * 1024),								/* Override receive buffer size */
kcConnect,								/* Called when a connection request is accepted */
kcDisconnect,							/* called when a connection becomes disconnected */
kcSend,									/* ctl_send_func - handles data sent from the client to kernel control  */
kcSetSocketOption,						/* called when the user process makes the setsockopt call */
kcGetSocketOption						/* called when the user process makes the getsockopt call */
};

bool 
Firewall::registerKernelControl()
{
	if(this->lockClientsQueue = IOLockAlloc())
	{
	
		if(!ctl_register(&kernelControlRegistration, &kernelControlReference))
		{
			return true;
		}
	
		IOLockFree(this->lockClientsQueue);
		this->lockClientsQueue = NULL;
	}
	
	IOLog("can't register kernel control");
	return false;
}

bool Firewall::unRegisterKernelControl()
{
	if(kernelControlReference == NULL)
		return true;

	//send to all close
	int counts = 3;
	while(clients)
	{
		if(!counts)
			return false;
		//Send firewall down
		send(NULL);
		IOSleep(200);
		counts--;
	}
	
	if(ctl_deregister(kernelControlReference))	
		return false;
	
	kernelControlReference = NULL;
	
	if(this->lockClientsQueue)
	{
		IOLockFree(this->lockClientsQueue);
		this->lockClientsQueue = NULL;
		
	}
	return true;
}

void 
Firewall::send(Message *message)
{
	if(!kernelControlReference || !this->lockClientsQueue)
		return;
	
	IOLockLock(this->lockClientsQueue);
	
	for(Client *curr = this->clients; curr; curr = curr->next)
		curr->Send(message);
	
	IOLockUnlock(this->lockClientsQueue);
}

void 
Firewall::sendTo(UInt32 unit, Message *message)
{
	IOLockLock(this->lockClientsQueue);

	for(Client *curr = this->clients; curr; curr = curr->next)
		if(curr->unit == unit)
			curr->Send(message);
	
	IOLockUnlock(this->lockClientsQueue);
}

errno_t 
Firewall::kcConnect(kern_ctl_ref kctlref, struct sockaddr_ctl *sac, void **unitinfo)
{
	Client *client = new Client();
	if(!client) return ENOMEM;
	
	client->initWithClient(kctlref, sac->sc_unit);	
	
	IOLockLock(Firewall::instance->lockClientsQueue);
	
	client->next = Firewall::instance->clients;
	Firewall::instance->clients = client;

	IOLockUnlock(Firewall::instance->lockClientsQueue);
	
	*unitinfo = client;
	return KERN_SUCCESS;
}

errno_t 
Firewall::kcDisconnect(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo)
{
	IOLockLock(Firewall::instance->lockClientsQueue);
	
	Client *prev = NULL;
	Client *curr = Firewall::instance->clients;
	
	while(curr)
	{
		if(curr->unit == unit && curr->kernelKontrolReference == kctlref)
		{
			if(prev)
				prev->next = curr->next;
			else
				Firewall::instance->clients = curr->next;
			
			curr->free();
			break;
		}
		
		prev = curr;
		curr = curr->next;
	}	
	
	IOLockUnlock(Firewall::instance->lockClientsQueue);
	return KERN_SUCCESS;
}

errno_t 
Firewall::kcSend(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, mbuf_t m, int flags)
{	
	return KERN_SUCCESS;
}

errno_t 
Firewall::kcSetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t len)
{
	//register for info and ui provider
	return KERN_SUCCESS;
}

errno_t 
Firewall::kcGetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t *len)
{
	return KERN_SUCCESS;
}
