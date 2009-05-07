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

#pragma mark socket filter functions
void	
Firewall::unregistered(sflt_handle handle)
{
	
}


errno_t	
Firewall::attach(void **cookie, socket_t so)
{
	SocketCookie *socketCookie = new SocketCookie();
	
	if(socketCookie == NULL)
	{
		//Log error
		return KERN_MEMORY_ERROR;
	}

	*cookie = socketCookie;
	//add to known sockets
	
	return KERN_SUCCESS;
}

void	
Firewall::detach(void *cookie, socket_t so)
{
	Firewall::instance->Send(NULL);
	if(cookie)
	{
		//remove from known socke
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
	OSIncrementAtomic8((SInt8*)cookie + 5);
	//watch_send_text_message("data in", so, cookie);
	return KERN_SUCCESS;
	
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
	OSIncrementAtomic8((SInt8*)cookie + 5);
	//watch_send_text_message("data out", so, cookie);
	return 0;
}


errno_t	
Firewall::connectIn(void *cookie, socket_t so, const struct sockaddr *from)
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
Firewall::connectOut(void *cookie, socket_t so, const struct sockaddr *to)
{
	OSIncrementAtomic8((SInt8*)cookie + 5);
	//watch_send_text_message("connect out", so, cookie);
	return 0;
}


errno_t	
Firewall::bind(void *cookie, socket_t so, const struct sockaddr *to)
{
	//	OSIncrementAtomic8(cookie + 5);
	//	watch_send_text_message("bind", so, cookie);
	return 0;
	
	//check is allowed to bind to that address
}


errno_t	
Firewall::setoption(void *cookie, socket_t so, sockopt_t opt)
{
	//OSIncrementAtomic8(cookie + 5);
	//watch_send_text_message("set option", so, cookie);
	return 0;
}

errno_t	
Firewall::getoption(void *cookie, socket_t so, sockopt_t opt)
{
	//	OSIncrementAtomic8(cookie + 5);
	//	watch_send_text_message("get optin", so, cookie);
	return 0;
}

errno_t	
Firewall::listen(void *cookie, socket_t so)
{
	//	OSIncrementAtomic8(cookie + 5);
	//	watch_send_text_message("listen", so, cookie);
	return 0;
	
	//that is for tcp //check is alowed listen
	//if not rule allow and process in accept function
}

errno_t	
Firewall::ioctl(void *cookie, socket_t so, u_int32_t request, const char* argp)
{
	//	OSIncrementAtomic8(cookie + 5);
	//	watch_send_text_message("ioctl", so, cookie);
	return 0;
}

errno_t 
Firewall::accept(void *cookie, socket_t so_listen, socket_t so, const struct sockaddr *local, const struct sockaddr *remote)
{
	//	OSIncrementAtomic8(cookie + 5);
	//	watch_send_text_message("accept", so, cookie);
	return 0;
	
	//check if allowed from that remote address
}




Firewall* 
Firewall::initInstance()
{
	Firewall* ptr = new Firewall();
	
	ptr->init();
	
	return ptr;
}

bool 
Firewall::Init()
{
	return true;
}

int 
Firewall::IsChanged(time_t)
{
	//if(this->) check for last time changed rules
	return 1;//not changed
}

Rule* 
Firewall::FindRule(const char* process_name, const char* process_path, 
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
Firewall::AddRule(Rule *rule)
{
	return NULL;
}

Rule* 
Firewall::DeleteRule(Rule *rule)
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
Firewall::RegisterKernelControl()
{
	errno_t retval = ctl_register(&kernelControlRegistration, &kernelControlReference);
	//LOG message
	return retval == KERN_SUCCESS;
}

bool Firewall::UnRegisterKernelControl()
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
		Send(NULL);
		IOSleep(200);
		counts--;
	}
	
	if(ctl_deregister(kernelControlReference))	
		return false;
	
	kernelControlReference = NULL;
	return true;
}

void 
Firewall::Send(Message *message)
{
	ClientNode *work = this->clients;
	
	IOLockLock(this->clientsLock);
	
	while(work)
		work->client->Send(message);
	
	IOLockUnlock(this->clientsLock);
}

void 
Firewall::SendTo(UInt32 unit, Message *message)
{
	ClientNode *work = this->clients;
	
	while(work)
	{
		if(work->client->unit == unit)
		{
			work->client->Send(message);
			break;
		}
	}
}

errno_t 
Firewall::kcConnect(kern_ctl_ref kctlref, struct sockaddr_ctl *sac, void **unitinfo)
{
	Client *client = new Client();
	if(!client) return ENOMEM;
	
	client->initWithClient(kctlref, sac->sc_unit, ClientStateActive);	
	
	ClientNode *node = new ClientNode();
	
	if(node == NULL)
	{
		client->free();
		return ENOMEM;
	}
	
	node->next = Firewall::instance->clients;
	Firewall::instance->clients = node;

	*unitinfo = client;
	return KERN_SUCCESS;
}

errno_t 
Firewall::kcDisconnect(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo)
{	
	ClientNode *prev = NULL;
	ClientNode *curr = Firewall::instance->clients;
	
	while(curr)
	{
		if(curr->client->unit == unit)
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
	return KERN_SUCCESS;
}

errno_t 
Firewall::kcGetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t *len)
{
	return KERN_SUCCESS;
}
