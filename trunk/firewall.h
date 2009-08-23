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
	Applications applications;
	SocketCookies socketCookies;
	
public:

	//static Firewall *instance;
	bool Init();
	bool Free();
	
#pragma mark soket filter functions

	bool RegisterSocketFilters();
	bool UnregisterSocketFilters();
	
	static protocol protocols[];
	static sflt_filter sfltFilter;
	
	static void		Unregistered(sflt_handle handle);
	static errno_t	Attach(void	**cookie, socket_t so);
	static void		Detach(void	*cookie, socket_t so);
	static void		Notify(void *cookie, socket_t so, sflt_event_t event, void *param);
	static int		GetPeerName(void *cookie, socket_t so, sockaddr **sa);
	static int		GetSockName(void *cookie, socket_t so, sockaddr **sa);
	static errno_t	DataIn(void *cookie, socket_t so, const sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags);
	static errno_t	DataOut(void *cookie, socket_t so, const sockaddr *to, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags);
	static errno_t	ConnectIn(void *cookie, socket_t so, const sockaddr *from);
	static errno_t	ConnectOut(void *cookie, socket_t so, const sockaddr *to);
	static errno_t	Bind(void *cookie, socket_t so, const sockaddr *to);
	static errno_t	SetOption(void *cookie, socket_t so, sockopt_t opt);
	static errno_t	GetOption(void *cookie, socket_t so, sockopt_t opt);
	static errno_t	Listen(void *cookie, socket_t so);
	static errno_t	Ioctl(void *cookie, socket_t so, u_int32_t request, const char* argp);
	static errno_t	Accept(void *cookie, socket_t so_listen, socket_t so, const sockaddr *local, const sockaddr *remote);
	
#pragma mark clent functions
	kern_ctl_ref kernelControlReference;
	static kern_ctl_reg kernelControlRegistration; 

	IOLock *lockClientsQueue;
	Client *clients;
	
	bool RegisterKernelControl();
	bool UnregisterKernelControl();
	
	static errno_t KcConnect(kern_ctl_ref kctlref, struct sockaddr_ctl *sac, void **unitinfo);
	static errno_t KcDisconnect(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo);
	static errno_t KcSend(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, mbuf_t m, int flags);
	static errno_t KcSetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t len);
	static errno_t KcGetSocketOption(kern_ctl_ref kctlref, u_int32_t unit, void *unitinfo, int opt, void *data, size_t *len);

	void Send(Message *message);
	void SendTo(UInt32 unit, Message *message);
};

extern Firewall firewall;

