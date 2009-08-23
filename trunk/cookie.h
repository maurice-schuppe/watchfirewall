#ifndef WATCH_COOKIE_H
#define WATCH_COOKIE_H

#include <sys/kpi_socketfilter.h>
#include <sys/mbuf.h>

#include "rule.h"
#include "application.h"


enum __attribute__((visibility("hidden"))) SocketCookieState
{
	SocketCookieStateNOT = 1,	
	SocketCookieStateASK = 2,	
	SocketCookieStateALLOWED = 4,	
	SocketCookieStateNOT_ALLOWED = 8
};

struct __attribute__((visibility("hidden"))) DeferredData
{
	mbuf_t data;
	mbuf_t control;
	sflt_data_flag_t flags;
	sockaddr *socketAddress;
	bool direction; //true out, false in
	
	DeferredData *next;
};

class __attribute__((visibility("hidden"))) SocketCookie
{
public:
	Application* application;
	SocketCookieState state;
	socket_t socket;
	UInt16 sockDomain;
	UInt16 sockType;
	UInt16 sockProtocol;	
	
	UInt64 aksRuelTime;
	UInt64 obtainedRuleTime;
	
	DeferredData *rootDefferedData;
	DeferredData *lastDefferedData;
	
	Rule *rule;
	
	struct sockaddr *fromAddress;
	struct sockaddr *toAddress;
	
	SocketCookie *prev;
	SocketCookie *next;
	
public:
	bool IsValid();
	
	void SetSocket(socket_t socket)
	{
		int domain;
		int type;
		int protocol;
		sock_gettype(socket, &domain, &type, &protocol);
		
		this->sockDomain = domain;
		this->sockType = type;
		this->sockProtocol = protocol;
		this->socket = socket;
	}
	
	void Free()
	{
		//TODO: free deferred data
		
		if(rule)
			rule->Release();
		
		if(application)
			application->Release();
		
		if(fromAddress)
			delete fromAddress;
		
		if(toAddress)
			delete toAddress;
		
		delete this;
		
	}
	
	bool SetFromAddress(const sockaddr *socketAddress);
	bool SetToAddress(const sockaddr *socketAddress);
	
	bool AddDeferredData(bool direction, mbuf_t data, mbuf_t control, sflt_data_flag_t flags, sockaddr *socketAddress);
	bool ClearDeferredData();
	bool SendDeferredData();
	
	
};

class __attribute__((visibility("hidden"))) SocketCookies
{
public:
	IOLock *lock;
	SocketCookie *socketCookies;
	static mbuf_tag_id_t mbufTagId;
	
public:
	SocketCookie * RemoveFromChain(SocketCookie *cookie)
	{
		//::IOLog("removed socket cookie\n");
		IOLockLock(lock);
		if(cookie->prev)
			cookie->prev->next = cookie->next;
		
		if(cookie->next)
			cookie->next->prev = cookie->prev;
		
		if(cookie == socketCookies)
			socketCookies = cookie->next;
		
		cookie->prev = cookie->next = NULL;
		
		IOLockUnlock(lock);
		
		return cookie;
	}
	
	void AddToChain(SocketCookie *cookie)
	{
		IOLockLock(lock);
		cookie->next = socketCookies;
		socketCookies = cookie;
		
		if(cookie->next)
			cookie->next->prev = cookie; 
		
		IOLockUnlock(lock);
	}
	
	bool Init()
	{
		if(lock == NULL)
		{
			IOLog("create socket cookie lock\n");
			lock = IOLockAlloc();
			if(lock)
			{
				if(mbuf_tag_id_find(MYBUNDLEID , &mbufTagId) == KERN_SUCCESS)
					return true;
			}
			IOLockFree(lock);			
		}
		
		return false;
	}
	
	bool Free()
	{
		if(!HaveAttachedSockets())
		{
			IOLockFree(lock);
			lock = NULL;
			return true;
		}
		
		return false;
	}
	
	bool HaveAttachedSockets()
	{
		bool result = false;
		IOLockLock(lock);
		result = (socketCookies != NULL);
		IOLockUnlock(lock);
		
		return result;
	}
	static bool PrependMbufHeader(mbuf_t *data, size_t pkt_len)
	{
		mbuf_t new_hdr;
		
		if (KERN_SUCCESS == mbuf_gethdr(MBUF_WAITOK, MBUF_TYPE_DATA, &new_hdr))
		{
			mbuf_setnext(new_hdr, *data);
			mbuf_setnextpkt(new_hdr, mbuf_nextpkt(*data));
			mbuf_pkthdr_setlen(new_hdr, pkt_len);
			mbuf_setlen(new_hdr, 0);
			mbuf_pkthdr_setrcvif(*data, NULL);
			
			*data = new_hdr;
			return true;
		}
		
		return false;
	}
	
	
	static bool	CheckTag(mbuf_t *m, int value)
	{
		errno_t	status;
		int		*tagReference;
		size_t	len;
		
		status = mbuf_tag_find(*m, mbufTagId, 1, &len, (void**)&tagReference);
		if ((status == 0) && (*tagReference == value) && (len == sizeof(value)))
			return true;
		
		return false;
	}
	
	static bool	SetTag(mbuf_t *data, int value)
	{	
		int		*tagReference = NULL;
		
		switch(mbuf_tag_allocate(*data, mbufTagId, 1, sizeof(value), MBUF_WAITOK, (void**)&tagReference))
		{
			case KERN_SUCCESS:
			{
				*tagReference = value;
				return true;
			}
				
			case EINVAL:
			{			
				mbuf_flags_t flags = mbuf_flags(*data);
				if ((flags & MBUF_PKTHDR) == 0)
				{
					size_t			totalbytes = 0;
					
					for (mbuf_t	m = *data; m; m = mbuf_next(m))
						totalbytes += mbuf_len(m);
					
					if (PrependMbufHeader(data, totalbytes))
					{
						if(mbuf_tag_allocate(*data, mbufTagId, 1, sizeof(value), MBUF_WAITOK, (void**)&tagReference) == KERN_SUCCESS)						
						{
							*tagReference = value;
							return true;
						}
					}
				}
			}
				
			default:
				return false;
		}	
	}
	
};


#endif