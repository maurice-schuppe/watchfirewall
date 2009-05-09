/*
 *  common.cpp
 *  Watch
 *
 *  Created by JanBird on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "common.h"

void collectSocketInfo(socket_t so, int *domain, int *type, int *protocol, pid_t *pid, uid_t *uid, char* procName, size_t procNameSize)
{
	sock_gettype(so, domain, type, protocol);
	
	proc_selfname(procName, procNameSize);
	
	*uid = kauth_getuid();
	*pid = proc_selfpid();
}

