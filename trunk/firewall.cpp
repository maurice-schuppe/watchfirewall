/*
 *  firewall.c
 *  Watch
 *
 *  Created by Jan Bird on 4/5/09.
 *  Copyright 2009 __MoonLight__. All rights reserved.
 *
 */

#include "firewall.h"


//OSDefineMetaClassAndStructors(Firewall, OSObject)


Firewall *Firewall::instance = NULL;



protocol Firewall::protocols[] =
{
{0xBACAF000, PF_INET, SOCK_STREAM, IPPROTO_TCP, 0},
{0xBACAF001, PF_INET, SOCK_DGRAM, IPPROTO_UDP, 0},
{0xBACAF002, PF_INET, SOCK_RAW, IPPROTO_ICMP, 0},
{0xBACAF003, PF_INET6, SOCK_STREAM, IPPROTO_TCP, 0},
{0xBACAF004, PF_INET6, SOCK_DGRAM, IPPROTO_UDP, 0},
{0xBACAF005, PF_INET6, SOCK_RAW, IPPROTO_ICMP, 0}/*,
{0xBACAF006, PF_UNIX, SOCK_STREAM, 0, 0}*/

};

sflt_filter Firewall::sfltFilter = {
0xBABABABC,					/* sflt_handle - use a registered creator type - <http://developer.apple.com/datatype/> */
SFLT_GLOBAL | SFLT_EXTENDED,/* sf_flags */
(char*)MYBUNDLEID,					/* sf_name - cannot be nil else param err results */
unregistered,	/* sf_unregistered_func */
attach,			/* sf_attach_func - cannot be nil else param err results */			
detach,			/* sf_detach_func - cannot be nil else param err results */
notify,			/* sf_notify_func */
getpeername,	/* sf_getpeername_func */
getsockname,	/* sf_getsockname_func */
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
	
	IOLog("unregister soket filters \n");
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
		IOLog("can't allocate memory for scoket cookie \n");
		return ENOMEM;
	}

	socketCookie->application = Application::getApplication();
	
	if(!socketCookie->application)
	{
		delete socketCookie;
		return ENOMEM;
	}
	
	socketCookie->setSocket(so);
	socketCookie->state = SocketCookieStateALLOWED;
	
	*cookie = socketCookie;
	socketCookie->addToChain();
	
	Message *messsage = Message::createTextFromCookie("attach", socketCookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	
	return KERN_SUCCESS;
}

void	
Firewall::detach(void *cookie, socket_t so)
{
	Message *messsage = Message::createTextFromCookie("detach", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();

	if(cookie)
		((SocketCookie*)cookie)->RemoveFromChain();
}

void	
Firewall::notify(void *cookie, socket_t so, sflt_event_t event, void *param)
{
	if(!Firewall::instance)
		return;
	
	Message *messsage = Message::createTextFromCookie("notify", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
}

int		
Firewall::getpeername(void *cookie, socket_t so, struct sockaddr **sa)
{
	if(!Firewall::instance)
		return KERN_SUCCESS;
	
	Message *messsage = Message::createTextFromCookie("getpername", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
}


int		
Firewall::getsockname(void *cookie, socket_t so, struct sockaddr **sa)
{
	Message *messsage = Message::createTextFromCookie("getsockname", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	

	return KERN_SUCCESS;
}


errno_t	
Firewall::dataIn(void *cookie, socket_t so, const struct sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{
	if(!Firewall::instance)
		return KERN_SUCCESS;

	Message *messsage = Message::createTextFromCookie("data in", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
	
	//check fo socket changes
	SocketCookie *scookie = (SocketCookie*)cookie;
	
	if(Firewall::instance->rules.isRulesChanged(scookie->obtainedRuleTime))
	{
		//update rule and set in cookie
		if(scookie->rule != NULL)
		{
			scookie->rule->release();
			scookie->rule = NULL;
			scookie->obtainedRuleTime = 0;
		}
		
		Rule* rule = Firewall::instance->rules.findRule( 
												  NULL, NULL, 
												  0, 0, 0, 
												  0, 
												  NULL);
		if(rule == NULL)
		{
			scookie->state == SocketCookieStateNOT;
		}
		
	}
	
	switch (scookie->state) 
	{
		case SocketCookieStateNOT:
			//send ask
			scookie->state = SocketCookieStateASK;
			//scookie->aks_rule_time = time();//TODO: insert header
			
		case SocketCookieStateASK:
			//cashe requiest
			return EJUSTRETURN;
		case SocketCookieStateALLOWED:
			return KERN_SUCCESS;
		case SocketCookieStateNOT_ALLOWED:
			return KERN_POLICY_LIMIT;//fix return value
		default:
			break;
	}
	
	return -1;
	
}


errno_t	
Firewall::dataOut(void *cookie, socket_t so, const struct sockaddr *to, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{
	if(!Firewall::instance)
		return KERN_SUCCESS;
	
	Message *messsage = Message::createTextFromCookie("data out", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
}


errno_t	
Firewall::connectIn(void *cookie, socket_t so, const struct sockaddr *from)
{
	SocketCookie *scookie = (SocketCookie*)cookie;
	
	Message *messsage = Message::createTextFromCookie("connect in", scookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
	
	//only for TCP/IP
	//copy from address
	scookie->SetFromAddress(from);

	if(Firewall::instance->firewallUp)
	{
		//check is have rule
		//if have apply
		//else allow and ask
		
	}
	
	
	
	if(Firewall::instance->countRegistredInfoSocket)
	{
		//send message connected
	}
}


errno_t	
Firewall::connectOut(void *cookie, socket_t so, const struct sockaddr *to)
{
	Message *messsage = Message::createTextFromCookie("connect out", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
}


errno_t	
Firewall::bind(void *cookie, socket_t so, const struct sockaddr *to)
{
	Message *messsage = Message::createTextFromCookie("bind", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	
	return KERN_SUCCESS;
	
	//check is allowed to bind to that address
}


errno_t	
Firewall::setoption(void *cookie, socket_t so, sockopt_t opt)
{
	Message *messsage = Message::createTextFromCookie("setoption", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
}

errno_t	
Firewall::getoption(void *cookie, socket_t so, sockopt_t opt)
{
	Message *messsage = Message::createTextFromCookie("getoption", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
}

errno_t	
Firewall::listen(void *cookie, socket_t so)
{
	Message *messsage = Message::createTextFromCookie("listen", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
	
	//that is for tcp //check is alowed listen
	//if not rule allow and process in accept function
}

errno_t	
Firewall::ioctl(void *cookie, socket_t so, u_int32_t request, const char* argp)
{
	Message *messsage = Message::createTextFromCookie("ioctl", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
}

errno_t 
Firewall::accept(void *cookie, socket_t so_listen, socket_t so, const struct sockaddr *local, const struct sockaddr *remote)
{
	Message *messsage = Message::createTextFromCookie("accept", (SocketCookie*)cookie);
	Firewall::instance->send(messsage);
	messsage->release();
	
	return KERN_SUCCESS;
	
	//check if allowed from that remote address
}




bool 
Firewall::init()
{
	if(!instance)
	{
		instance = new Firewall();
		
		if(!instance)
			return false;

		instance->closing = false;
		
		SocketCookie::init();
		
		instance->rules.init();
		
		if(!Application::initStatic())
			return false;
		
		if(instance->registerSocketFilters())
		{
			if(instance->registerKernelControl())
			{
				IOLog("instance created \n");
				return true;
			}
			
			instance->unregisterSocketFilters();
		}
		
		Application::freeStatic();
		delete instance;
		return false;
	}
	
	IOLog("firewall instance exist \n");
	return true;
}

bool
Firewall::free()
{
	IOLog("firewall instance begin destroed \n");

	if(instance == NULL)
		return true;

	instance->closing = true;
	
	if(!instance->unregisterSocketFilters())
		return false;
	
	//wait to close open sockets
	
	//check for connections
	if(!instance->unRegisterKernelControl())
		return false;

	if(!SocketCookie::free())
		return false;
	
	Application::freeStatic();
	instance->rules.free();
	
	delete instance;
	
	IOLog("firewall instance destroed \n");
	
	return true;
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

	Message *message = Message::createFirewallClose();
	if(!message)
		return false;
	
	int counts = 3;
	while(clients)
	{
		if(!counts)
			return false;
		//Send firewall down
		send(message);
		IOSleep(200);
		counts--;
	}
	
	message->release();
	
	if(ctl_deregister(kernelControlReference))	
		return false;
	
	IOLog("unregister kernel control \n");
	
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
	if(!Firewall::instance  || Firewall::instance->closing )
		return KERN_FAILURE;
	
	IOLog("Client joined \n");
	
	Client *client = new Client();
	if(!client) return ENOMEM;
	
	if(client->initWithClient(kctlref, sac->sc_unit) == false)
	{
		IOLog("can't create client");
		delete client;
		return ENOMEM;
	}
	
	IOLockLock(Firewall::instance->lockClientsQueue);
	
	client->next = Firewall::instance->clients;
	Firewall::instance->clients = client;

	IOLockUnlock(Firewall::instance->lockClientsQueue);
	
	*unitinfo = client;
	IOLog("Client registred \n");
	return KERN_SUCCESS;
}

errno_t 
Firewall::kcDisconnect(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo)
{
	IOLog("Client disconecting \n");
	//return KERN_SUCCESS;

	
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
			
			curr->closeSignal();
			curr->release();
			break;
		}
		
		prev = curr;
		curr = curr->next;
	}	
	
	IOLockUnlock(Firewall::instance->lockClientsQueue);
	IOLog("Client disconected \n");
	return KERN_SUCCESS;
}

errno_t 
Firewall::kcSend(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, mbuf_t m, int flags)
{	
	return KERN_SUCCESS;

	Client *client = (Client*)unitinfo;
	
	if(Firewall::instance == NULL || Firewall::instance->closing || client == NULL)
		return KERN_TERMINATED;
	
	
	UInt32 currentPosition = 0;
	
	size_t dataSize= mbuf_len(m);
	MessageBase *message = (MessageBase*)mbuf_data(m);
	
	while(currentPosition < dataSize)
	{
		//message.buffer = data + currentPosition;
		switch (message->type)
		{
			case MessageTypeDeleteRule:
				Firewall::instance->rules.deleteRule(((MessageDeleteRule*)message)->id);
				break;
				
			case MessageTypeAddRule:
				{
					Rule *rule = Firewall::instance->rules.addRule((MessageAddRule*)message);
					if(rule)
					{
						//send rule added
						
						rule->release();
					}
				}
				break;
				
			case MessageTypeActivateRule:
				Firewall::instance->rules.activateRule(((MessageActivateRule*)message)->id);
				break;
				
			case MessageTypeDeactivateRule:
				Firewall::instance->rules.deactivateRule(((MessageDeactivateRule*)message)->id);
				break;
				
			case MessageTypeActivateFirewall:
				break;
				
			case MessageTypeDeactivateFirewall:
				break;
				
			case MessageTypeRegisterForAsk:
				if(client->registerMessageClasses(MessageClassAsk))
					OSIncrementAtomic(&Firewall::instance->countRegistredAsk);
				break;
				
			case MessageTypeUnregisterAsk:
				if(client->unregisterMessageClasses(MessageClassAsk))
					OSDecrementAtomic(&Firewall::instance->countRegistredAsk);
				break;
				
			case MessageTypeRegisterForInfoRule:
				if(client->registerMessageClasses(MessageClassInfoRule))
					OSIncrementAtomic(&Firewall::instance->countRegistredInfoRule);
				break;

			case MessageTypeUnregisterInfoRule:
				if(client->unregisterMessageClasses(MessageClassInfoRule))
					OSDecrementAtomic(&Firewall::instance->countRegistredInfoRule);
				break;
				
			case MessageTypeRegisterForInfoSocket:
				if(client->registerMessageClasses(MessageClassInfoSocket))
					OSIncrementAtomic(&Firewall::instance->countRegistredInfoSocket);
				break;
				
			case MessageTypeUnregisterInfoSocket:
				if(client->unregisterMessageClasses(MessageClassInfoSocket))
					OSDecrementAtomic(&Firewall::instance->countRegistredInfoSocket);
				break;
		}
		
		currentPosition += message->size;
		message = (MessageBase*)((char*)message + message->size);
	}
	
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
