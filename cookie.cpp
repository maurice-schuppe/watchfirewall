/*
 *  cookie.c
 *  Watch
 *
 *  Created by Jan Bird on 4/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "cookie.h"
#include <sys/proc.h>

IOLock *SocketCookie::lock;
SocketCookie *SocketCookie::socketCookies;


//used only if exit extension
bool
SocketCookie::isValid()
{
	//check socket
	
	//check last pid used
	proc_t p;
	p = proc_find(this->application->pid);
	if (p)
	{
		proc_rele(p);
		return true;
	}
	return false;
}