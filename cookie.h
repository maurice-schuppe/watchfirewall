/*
 *  cookie.h
 *  Watch
 *
 *  Created by Jan Bird on 4/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef WATCH_COOKIE_H
#define WATCH_COOKIE_H

#include "rule.h"
#include "application.h"
#include <sys/kpi_socketfilter.h>

enum SocketCookieState
{
	SocketCookieStateNOT = 1,	
	SocketCookieStateASK = 2,	
	SocketCookieStateALLOWED = 4,	
	SocketCookieStateNOT_ALLOWED = 8
};

struct DeferredData
{
	mbuf_t data;
	mbuf_t control;
	sflt_data_flag_t flags;
	sockaddr *socketAddress;
	bool direction; //true out, false in
	
	DeferredData *next;
};

class SocketCookie
{
public:
	static IOLock *lock;
	static SocketCookie *socketCookies;
	
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
	bool isValid();
	
	void setSocket(socket_t socket)
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
	
	bool SetFromAddress(const sockaddr *socketAddress);
	bool SetToAddress(const sockaddr *socketAddress);
	
	void RemoveFromChain()
	{
		IOLockLock(lock);
		if(prev)
			prev->next = next;
		
		if(next)
			next->prev = prev;
		
		prev = next = NULL;
		if(this == socketCookies)
			socketCookies = NULL;

		IOLockUnlock(lock);

		if(this->rule)
			this->rule->release();
		
		if(this->application)
			this->application->release();
		
		
		delete this;
	}
	
	void addToChain()
	{
		IOLockLock(lock);
		this->next = socketCookies;
		socketCookies = this;
		
		if(this->next)
			this->next->prev = this; 
		
		IOLockUnlock(lock);
	}
	
	static bool init()
	{
		if(lock == NULL)
		{
			IOLog("create socket cookie lock\n");
			lock = IOLockAlloc();
			if(!lock)
				return false;
		}
		
		return true;
	}
	
	static bool free()
	{
		if(!haveAttachedSockets())
		{
			IOLockFree(lock);
			lock = NULL;
			return true;
		}
		
		return false;
	}
	
	static bool haveAttachedSockets()
	{
		bool result = false;
		IOLockLock(lock);
		result = (socketCookies != NULL);
		IOLockUnlock(lock);
		
		return result;
	}
};



#endif