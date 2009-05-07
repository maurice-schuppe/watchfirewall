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

enum SocketCookieState
{
	CookieStateNOT = 1,	
	CookieStateASK = 2,	
	CookieStateALLOWED = 4,	
	CookieStateNOT_ALLOWED = 8
};

class SocketCookie: public OSObject
{
	OSDeclareDefaultStructors(SocketCookie)

public:
	pid_t pid;
	pid_t p_pid;
	uid_t uid;	
	SocketCookieState state;
	
	time_t aks_time;
	
	Rule *rule;
	
	struct sockaddr *from_address;
	struct sockaddr *to_address;
	
public:
	virtual void free();
	
};

#endif