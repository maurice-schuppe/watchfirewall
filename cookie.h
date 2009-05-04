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

#define COOKIE_RULE_STATE_NOT 1
#define COOKIE_RULE_STATE_ASK 2
#define COOKIE_RULE_STATE_ALLOWED 4
#define COOKIE_RULE_STATE_NOT_ALLOWED 8

typedef struct _watch_socket_cookie
{
	pid_t process_pid;
	pid_t parent_process_pid;
	uid_t user_uid;	
	int8_t state;
	
	time_t aks_rule_time;
	
	watch_rule *rule_reference;
	
	struct sockaddr *from_address;
	struct sockaddr *to_address;
	
} watch_socket_cookie;

#endif