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
	
	bool SetFromAddress(const sockaddr *socketAddress);
	bool SetToAddress(const sockaddr *socketAddress);
	
	void RemoveFromChain()
	{
		//::IOLog("removed socket cookie\n");
		IOLockLock(lock);
		if(prev)
			prev->next = next;
		
		if(next)
			next->prev = prev;

		if(this == socketCookies)
			socketCookies = next;
		
		prev = next = NULL;

		IOLockUnlock(lock);

		if(this->rule)
			this->rule->Release();
		
		if(this->application)
			this->application->Release();
		
		
		delete this;
	}
	
	void AddToChain()
	{
		//::IOLog("added socket cookie\n");
		IOLockLock(lock);
		this->next = socketCookies;
		socketCookies = this;
		
		if(this->next)
			this->next->prev = this; 
		
		IOLockUnlock(lock);
	}
	
	static bool InitGlobal()
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
	
	static bool FreeGlobal()
	{
		if(!HaveAttachedSockets())
		{
			IOLockFree(lock);
			lock = NULL;
			return true;
		}
		
		return false;
	}
	
	static bool HaveAttachedSockets()
	{
		bool result = false;
		IOLockLock(lock);
		result = (socketCookies != NULL);
		IOLockUnlock(lock);
		
		return result;
	}
};



#endif