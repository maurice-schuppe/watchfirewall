/*
 *  cookie.c
 *  Watch
 *
 *  Created by Jan Bird on 4/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "cookie.h"

#define super    OSObject

OSDefineMetaClassAndStructors(SocketCookie, OSObject);

void
SocketCookie::free()
{
	if(rule) rule->release();
	super::free();
}