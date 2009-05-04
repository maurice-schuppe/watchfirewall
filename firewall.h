/*
 *  firewall.h
 *  Watch
 *
 *  Created by Jan Bird on 4/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "rule.h"

typedef struct _watch_firewall
{
	int firewall_up;
	int monitor_up;
	int count_attached_sockets;
	int capacity_rules;
	int count_rules;
	watch_rule *rules;
	
} watch_firewall;

extern watch_firewall firewall;

int watch_firewall_rules_changed();
watch_rule* watch_firewall_find_rule(watch_firewall *firewall, 
									 const char* process_name, const char* process_path, 
									 u_int16_t sock_famely, u_int16_t socket_type, u_int16_t sock_protocol, 
									 u_int8_t direction, struct sockaddr *sockaddres );


watch_rule* watch_firewall_add_rule(watch_firewall *this, watch_rule *rule);
watch_rule* watch_firewall_remove_rule(watch_firewall *this, watch_rule *rule);
