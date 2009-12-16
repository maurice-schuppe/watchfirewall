/*
 *  firewall.c
 *  Watch
 *
 *  Created by Jan Bird on 4/5/09.
 *  Copyright 2009 __MoonLight__. All rights reserved.
 *
 */

#include <sys/kpi_socket.h>
#include "firewall.h"

Firewall firewall;

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
Unregistered,	/* sf_unregistered_func */
Attach,			/* sf_attach_func - cannot be nil else param err results */			
Detach,			/* sf_detach_func - cannot be nil else param err results */
Notify,			/* sf_notify_func */
GetPeerName,	/* sf_getpeername_func */
GetSockName,	/* sf_getsockname_func */
DataIn,			/* sf_data_in_func */
DataOut,		/* sf_data_out_func */
ConnectIn,		/* sf_connect_in_func */
ConnectOut,		/* sf_connect_out_func */
Bind,			/* sf_bind_func */
SetOption,		/* sf_setoption_func */
GetOption,		/* sf_getoption_func */
Listen,			/* sf_listen_func */
Ioctl,			/* sf_ioctl_func */
{sizeof(sflt_filter::sflt_filter_ext), Accept, {NULL,NULL,NULL,NULL,NULL}} /*sf_filter_ext */
};

bool 
Firewall::RegisterSocketFilters()
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
Firewall::UnregisterSocketFilters()
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
Firewall::Unregistered(sflt_handle handle)
{
	
}


errno_t	
Firewall::Attach(void **cookie, socket_t so)
{
	//TODO: test
	//return ENOMEM;

	if(firewall.closing)
		return ENOMEM;
	
	SocketCookie *socketCookie = new SocketCookie;
	
	if(socketCookie == NULL)
	{
		IOLog("can't allocate memory for scoket cookie \n");
		return ENOMEM;
	}

	socketCookie->application = firewall.applications.GetApplication();
	
	if(!socketCookie->application)
	{
		delete socketCookie;
		return ENOMEM;
	}
	
	socketCookie->SetSocket(so);
	socketCookie->state = SocketCookieStateALLOWED;
	
	*cookie = socketCookie;
	firewall.socketCookies.AddToChain(socketCookie);
	
	Message *messsage = MessageText::CreateFromCookie("attach", socketCookie);
	firewall.Send(messsage);
	messsage->Release();
	
	
	return KERN_SUCCESS;
}

void	
Firewall::Detach(void *cookie, socket_t so)
{
	Message *messsage = MessageText::CreateFromCookie("detach", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();

	if(cookie)
	{
		firewall.socketCookies.RemoveFromChain((SocketCookie*)cookie)->Free();
	}
}

void	
Firewall::Notify(void *cookie, socket_t so, sflt_event_t event, void *param)
{
	
	Message *messsage = MessageText::CreateFromCookie("notify", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
}

int		
Firewall::GetPeerName(void *cookie, socket_t so, struct sockaddr **sa)
{
	Message *messsage = MessageText::CreateFromCookie("getpername", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
}


int		
Firewall::GetSockName(void *cookie, socket_t so, struct sockaddr **sa)
{
	Message *messsage = MessageText::CreateFromCookie("getsockname", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	

	return KERN_SUCCESS;
}


errno_t	
Firewall::DataIn(void *cookie, socket_t so, const sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{

	char buffer[30];
	SocketCookie *scookie = (SocketCookie*)cookie;
	
	if(scookie == NULL)
		return KERN_SUCCESS;
	
	//int size = 30;
	sockaddr *sa = (sockaddr*) &buffer;
	if(KERN_SUCCESS == sock_getsockname(so, sa, 30))
	{
		//print name
		//IOLog("sock address: %s", sa);
	}
	
	Message *messsage = MessageText::CreateFromCookie("data in", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
	
	//check fo socket changes

	
	if(firewall.rules.IsRulesChanged(scookie->obtainedRuleTime))
	{
		//update rule and set in cookie
		if(scookie->rule != NULL)
		{
			scookie->rule->Release();
			scookie->rule = NULL;
			scookie->obtainedRuleTime = 0;
		}
		
		Rule* rule = firewall.rules.FindRule( 
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
Firewall::DataOut(void *cookie, socket_t so, const sockaddr *to, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags)
{
	Message *messsage = MessageText::CreateFromCookie("data out", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
}


errno_t	
Firewall::ConnectIn(void *cookie, socket_t so, const sockaddr *from)
{

	SocketCookie *scookie = (SocketCookie*)cookie;
	
	Message *messsage = MessageText::CreateFromCookie("connect in", scookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
	
	//only for TCP/IP
	//copy from address
	scookie->SetFromAddress(from);

	if(firewall.firewallUp)
	{
		//check is have rule
		//if have apply
		//else allow and ask
		
	}
	
	
	
	if(firewall.countRegistredInfoSocket)
	{
		//send message connected
	}
}


errno_t	
Firewall::ConnectOut(void *cookie, socket_t so, const sockaddr *to)
{
	
	Message *messsage = MessageText::CreateFromCookie("connect out", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
}


errno_t	
Firewall::Bind(void *cookie, socket_t so, const sockaddr *to)
{

	Message *messsage = MessageText::CreateFromCookie("bind", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	
	return KERN_SUCCESS;
	
	//check is allowed to bind to that address
}


errno_t	
Firewall::SetOption(void *cookie, socket_t so, sockopt_t opt)
{

	Message *messsage = MessageText::CreateFromCookie("setoption", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
}

errno_t	
Firewall::GetOption(void *cookie, socket_t so, sockopt_t opt)
{
	Message *messsage = MessageText::CreateFromCookie("getoption", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
}

errno_t	
Firewall::Listen(void *cookie, socket_t so)
{

	Message *messsage = MessageText::CreateFromCookie("listen", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
	
	//that is for tcp //check is alowed listen
	//if not rule allow and process in accept function
}

errno_t	
Firewall::Ioctl(void *cookie, socket_t so, u_int32_t request, const char* argp)
{

	Message *messsage = MessageText::CreateFromCookie("ioctl", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
}

errno_t 
Firewall::Accept(void *cookie, socket_t so_listen, socket_t so, const struct sockaddr *local, const struct sockaddr *remote)
{
	Message *messsage = MessageText::CreateFromCookie("accept", (SocketCookie*)cookie);
	firewall.Send(messsage);
	messsage->Release();
	
	return KERN_SUCCESS;
	
	//check if allowed from that remote address
}




bool 
Firewall::Init()
{
	closing = false;
	
	if(socketCookies.Init())
	{	
		if(rules.Init())
		{
			if(applications.Init())
			{	
				if(RegisterSocketFilters())
				{
					if(RegisterKernelControl())
					{
						IOLog("instance created \n");
						return true;
					}
					
					UnregisterSocketFilters();
				}
				applications.Free();
			}
			rules.Free();
			
		}
		socketCookies.Free();
	}
	
	return false;
}

bool
Firewall::Free()
{
	IOLog("firewall instance begin destroed \n");

	closing = true;

	IOLog("firewall instance begin destroed socket filters \n");
	
	if(!UnregisterSocketFilters())
		return false;
	
	//check for connections
	if(!UnregisterKernelControl())
		return false;

	while(socketCookies.Free() == false)
		IOSleep(1);

	IOLog("firewall instance begin destroed applications \n");
	applications.Free();

	rules.Free();

	//IOSleep(200);
	
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
KcConnect,								/* Called when a connection request is accepted */
KcDisconnect,							/* called when a connection becomes disconnected */
KcSend,									/* ctl_send_func - handles data sent from the client to kernel control  */
KcSetSocketOption,						/* called when the user process makes the setsockopt call */
KcGetSocketOption						/* called when the user process makes the getsockopt call */
};

bool 
Firewall::RegisterKernelControl()
{
	IOLog("clients %p\n", clients);
	
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

bool Firewall::UnregisterKernelControl()
{
	if(kernelControlReference == NULL)
		return true;

	Message *message = MessageFirewallClosing::Create();
	if(!message)
		return false;
	
	int counts = 3;
	while(clients)
	{
		if(!counts)
			return false;
		//Send firewall down
		Send(message);
		IOSleep(200);
		counts--;
	}
	
	message->Release();
	
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
Firewall::Send(Message *message)
{
	if(!kernelControlReference || !this->lockClientsQueue)
		return;
	
	IOLockLock(this->lockClientsQueue);
	
	for(Client *client = this->clients; client; client = client->next)
		client->Send(message);
	
	IOLockUnlock(this->lockClientsQueue);
}

void 
Firewall::SendTo(UInt32 unit, Message *message)
{
	IOLockLock(this->lockClientsQueue);

	for(Client *curr = this->clients; curr; curr = curr->next)
		if(curr->unit == unit)
			curr->Send(message);
	
	IOLockUnlock(this->lockClientsQueue);
}

errno_t 
Firewall::KcConnect(kern_ctl_ref kctlref, struct sockaddr_ctl *sac, void **unitinfo)
{
	if(firewall.closing )
		return KERN_FAILURE;
	
	IOLog("Client joined \n");
	
	Client *client = new Client();
	if(!client) return ENOMEM;
	
	if(client->InitWithClient(kctlref, sac->sc_unit) == false)
	{
		IOLog("can't create client");
		delete client;
		return ENOMEM;
	}
	
	IOLockLock(firewall.lockClientsQueue);
	
	client->next = firewall.clients;
	firewall.clients = client;

	IOLockUnlock(firewall.lockClientsQueue);
	
	*unitinfo = client;
	IOLog("Client registred \n");
	return KERN_SUCCESS;
}

errno_t 
Firewall::KcDisconnect(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo)
{
	IOLog("Client disconnecting \n");
	//return KERN_SUCCESS;

	
	IOLockLock(firewall.lockClientsQueue);
	
	Client *prev = NULL;
	Client *curr = firewall.clients;
	
	while(curr)
	{
		if(curr->unit == unit && curr->kernelKontrolReference == kctlref)
		{
			if(prev)
				prev->next = curr->next;
			else
				firewall.clients = curr->next;
			
			curr->CloseSignal();
			//update counts of listen etc...
			if(curr->registredMessageClases & MessageClassAsk)
				OSDecrementAtomic(&firewall.countRegistredAsk);

			if(curr->registredMessageClases & MessageClassInfoRule)
				OSDecrementAtomic(&firewall.countRegistredInfoRule);
			
			if(curr->registredMessageClases & MessageClassInfoSocket)
				OSDecrementAtomic(&firewall.countRegistredInfoSocket);

			curr->Release();
			break;
		}
		
		prev = curr;
		curr = curr->next;
	}	
	
	IOLockUnlock(firewall.lockClientsQueue);
	IOLog("Client disconected \n");
	return KERN_SUCCESS;
}

errno_t 
Firewall::KcSend(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, mbuf_t m, int flags)
{	
	Client *client = (Client*)unitinfo;
	
	if(firewall.closing || client == NULL)
		return KERN_TERMINATED;
	
	UInt32 currentPosition = 0;
	
	size_t dataSize= mbuf_len(m);
	RawMessageClientAction *message = (RawMessageClientAction*) mbuf_data(m);
	
	while(currentPosition < dataSize)
	{
		switch (message->type)
		{
			case MessageTypeDeleteRule:
				{
					RawMessageDeleteRule *rawMessageDeleteRule = (RawMessageDeleteRule*) message;
					
					MessageRuleDeleted* responce = MessageRuleDeleted::Create(client->unit, message->messageId, 0, rawMessageDeleteRule->ruleId);
					if(responce == NULL)
						break;
					
					Rule *rule; //????
					switch(firewall.rules.DeleteRule(rawMessageDeleteRule->ruleId, &rule))
					{
						case 0://OK
							firewall.Send(responce);
							break;
						case 1://not exist
							responce->rawMessage.actionState = 1;
							client->Send(responce);
							break;
					}
					
					if(rule)
						rule->Release();
					
					responce->Release();
				}
				break;
				
			case MessageTypeAddRule:
				{
					RawMessageAddRule* rawMessageAddRule = (RawMessageAddRule*) message;
					
					//TODO: refactor message
					MessageRuleAdded* responce = MessageRuleAdded::Create(client->unit, rawMessageAddRule->id, 0, 0);
					
					Rule *rule = NULL;
					switch(firewall.rules.AddRule(rawMessageAddRule, &rule))
					{
						case -1://memory error
							responce->rawMessage.actionState = -1;
							client->Send(responce);
						   break;
						case 0://ok
							//TODO: fill new rule data
							firewall.Send(responce);
						   break;
						case 1://already exist
							responce->rawMessage.actionState = 1;
							client->Send(responce);
						   break;
					}
						   
					if(rule)
						   rule->Release();
					
					responce->Release();
				}
				break;
				
			case MessageTypeActivateRule:
				{
					Rule *rule;
					switch(firewall.rules.ActivateRule(((RawMessageActivateRule*)message)->ruleId, &rule))
					{
						case -1://not exist
						   break;
						case 0://ok
						   break;
						case 1://already activated
						   break;
					}
					if(rule)
						   rule->Release();
				}
				break;
				
			case MessageTypeDeactivateRule:
				{
					Rule *rule;
					switch(firewall.rules.DeactivateRule(((RawMessageDeactivateRule*)message)->ruleId, &rule))
					{
						case -1://not exist
							break;
						case 0://ok
							break;
						case 1://already deacivated
							break;
					}
					
					if(rule)
						rule->Release();
				}
				break;
				
			case MessageTypeActivateFirewall:
				{
					MessageFirewallActivated* responce = MessageFirewallActivated::Create(client->unit, message->messageId, 0);
					if(responce == NULL)
						break;
					
					if(firewall.firewallUp == false)
					{
						firewall.firewallUp = true;
						//send message
						responce->rawMessage.actionState = 1;
						
					}
					firewall.Send(responce);
					responce->Release();
				}
				break;
				
			case MessageTypeDeactivateFirewall:
				{
					MessageFirewallActivated* responce = MessageFirewallActivated::Create(client->unit, message->messageId, 0);
					if(responce == NULL)
						break;

					if(firewall.firewallUp == true)
					{
						firewall.firewallUp = false;
						responce->rawMessage.actionState = 1;
					}

					firewall.Send(responce);
					responce->Release();
				}
				break;
				
			case MessageTypeRegisterForAsk:
				{
					MessageRegistredForAsk* responce = MessageRegistredForAsk::Create(client->unit, message->messageId, 0);
					if(responce == NULL)
						break;
					
					if(client->RegisterMessageClasses(MessageClassAsk))
						OSIncrementAtomic(&firewall.countRegistredAsk);
					else
						responce->rawMessage.actionState = 1;
					
					client->Send(responce);
					responce->Release();
				}
				break;
				
			case MessageTypeUnregisterAsk:
				{
					MessageUnregistredAsk* responce = MessageUnregistredAsk::Create(client->unit, message->messageId, 0);
					if(responce == NULL)
						break;
					
					if(client->UnregisterMessageClasses(MessageClassAsk))
						OSDecrementAtomic(&firewall.countRegistredAsk);
					else
						responce->rawMessage.actionState = 1;
					
					client->Send(responce);
					responce->Release();
				}
				break;
				
			case MessageTypeRegisterForInfoRule:
				{
					MessageRegistredForInfoRule* responce = MessageRegistredForInfoRule::Create(client->unit, message->messageId, 0);
					if(responce == NULL)
						break;
					
					if(client->RegisterMessageClasses(MessageClassInfoRule))
						OSIncrementAtomic(&firewall.countRegistredInfoRule);
					else
						responce->rawMessage.actionState = 1;
					
					client->Send(responce);
					responce->Release();
				}
				break;

			case MessageTypeUnregisterInfoRule:
				{
					MessageUnregistredInfoRule* responce = MessageUnregistredInfoRule::Create(client->unit, message->messageId, 0);
					if(responce == NULL)
						break;
					
					if(client->UnregisterMessageClasses(MessageClassInfoRule))
						OSDecrementAtomic(&firewall.countRegistredInfoRule);
					else
						responce->rawMessage.actionState = 1;

					client->Send(responce);
					responce->Release();
				}
				break;
				
			case MessageTypeRegisterForInfoSocket:
				{
					MessageRegistredForInfoRule* responce = MessageRegistredForInfoRule::Create(client->unit, message->messageId, 0);
					if(responce == NULL)
						break;
					
					if(client->RegisterMessageClasses(MessageClassInfoSocket))
						OSIncrementAtomic(&firewall.countRegistredInfoSocket);
					else
						responce->rawMessage.actionState = 1;
						//((RawMessageRegistredForInfoSocket*) &responce->m)->actionState = 1;

					client->Send(responce);
					responce->Release();
				}
				break;
				
			case MessageTypeUnregisterInfoSocket:
				{
					MessageUnregistredInfoSocket* responce = MessageUnregistredInfoSocket::Create(client->unit, message->messageId, 0);
					if(responce ==NULL)
						break;
					
					if(client->UnregisterMessageClasses(MessageClassInfoSocket))
						OSDecrementAtomic(&firewall.countRegistredInfoSocket);
					else
						responce->rawMessage.actionState = 1;
					
					client->Send(responce);
					responce->Release();
				}
				break;
		}
		
		currentPosition += message->size;
		message = (RawMessageClientAction*)((char*)message + message->size);
	}
	
	return KERN_SUCCESS;
}

errno_t 
Firewall::KcSetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t len)
{
	//register for info and ui provider
	return KERN_SUCCESS;
}

errno_t 
Firewall::KcGetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t *len)
{
	return KERN_SUCCESS;
}
