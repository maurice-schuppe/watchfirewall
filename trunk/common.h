/*
 *  common.h
 *  Watch
 *
 *  Created by JanBird on 5/8/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <sys/kernel_types.h>
#include <sys/kpi_socket.h>
#include <sys/proc.h>
#include <sys/kauth.h>

void collectSocketInfo(socket_t so, int *domain, int *type, int *protocol, pid_t *pid, uid_t *uid, char* procName, size_t procNameSize);
