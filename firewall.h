/*
 *  firewall.h
 *  Watch
 *
 *  Created by Jan Bird on 4/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

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
#include <sys/kern_control.h>
#include <IOKit/IOLib.h>
#include <IOKit/IOLocks.h>

#include "rule.h"
#include "cookie.h"
#include "messages.h"
#include "client.h"

#define MYBUNDLEID "com.janbird.watch.kext"

struct __attribute__((visibility("hidden"))) protocol
{
	sflt_handle handle;
	int domain;
	int type;
	int protocol;
	int state;
};

class __attribute__((visibility("hidden"))) Firewall 
{
public:
	bool	firewallUp;
	bool	closing;
	UInt32	countAttachedSockets;
	
	volatile SInt32 countRegistredInfoSocket;
	volatile SInt32 countRegistredInfoRule;
	volatile SInt32 countRegistredAsk;
	
	Rules	rules;
	//Applications applications;
	
public:

	static Firewall *instance;
	static bool init();
	static bool free();
	
#pragma mark soket filter functions

	bool registerSocketFilters();
	bool unregisterSocketFilters();
	
	static protocol protocols[];
	static sflt_filter sfltFilter;
	
	static void		unregistered(sflt_handle handle);
	static errno_t	attach(void	**cookie, socket_t so);
	static void		detach(void	*cookie, socket_t so);
	static void		notify(void *cookie, socket_t so, sflt_event_t event, void *param);
	static int		getpeername(void *cookie, socket_t so, struct sockaddr **sa);
	static int		getsockname(void *cookie, socket_t so, struct sockaddr **sa);
	static errno_t	dataIn(void *cookie, socket_t so, const struct sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags);
	static errno_t	dataOut(void *cookie, socket_t so, const struct sockaddr *to, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags);
	static errno_t	connectIn(void *cookie, socket_t so, const struct sockaddr *from);
	static errno_t	connectOut(void *cookie, socket_t so, const struct sockaddr *to);
	static errno_t	bind(void *cookie, socket_t so, const struct sockaddr *to);
	static errno_t	setoption(void *cookie, socket_t so, sockopt_t opt);
	static errno_t	getoption(void *cookie, socket_t so, sockopt_t opt);
	static errno_t	listen(void *cookie, socket_t so);
	static errno_t	ioctl(void *cookie, socket_t so, u_int32_t request, const char* argp);
	static errno_t	accept(void *cookie, socket_t so_listen, socket_t so, const struct sockaddr *local, const struct sockaddr *remote);
	
#pragma mark clent functions
	kern_ctl_ref kernelControlReference;
	static kern_ctl_reg kernelControlRegistration; 

	IOLock *lockClientsQueue;
	Client *clients;
	
	bool registerKernelControl();
	bool unRegisterKernelControl();
	
	static errno_t kcConnect(kern_ctl_ref kctlref, struct sockaddr_ctl *sac, void **unitinfo);
	static errno_t kcDisconnect(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo);
	static errno_t kcSend(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, mbuf_t m, int flags);
	static errno_t kcSetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t len);
	static errno_t kcGetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t *len);

	void send(Message *message);
	void sendTo(UInt32 unit, Message *message);
};

