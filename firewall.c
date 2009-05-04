/*
 *  firewall.c
 *  Watch
 *
 *  Created by Jan Bird on 4/5/09.
 *  Copyright 2009 __MoonLight__. All rights reserved.
 *
 */

#include "firewall.h"

watch_firewall firewall = {0,0,0,0,0,NULL};//initilaiziation

int watch_firewall_rules_changed(watch_firewall *this)
{
	//if(this->) check for last time changed rules
	return 1;//not changed
}

watch_rule* watch_firewall_find_rule(watch_firewall *firewall, 
									 const char* process_name, const char* process_path, 
									 u_int16_t sock_famely, u_int16_t socket_type, u_int16_t sock_protocol, 
									 u_int8_t direction, struct sockaddr *sockaddres )
{
	//TODO: refactor
	watch_rule *current_rule = firewall->rules;
	
	while(current_rule)
	{		
	}
	
	return NULL;
}


watch_rule* watch_firewall_add_rule(watch_firewall *this, watch_rule *rule)
{
	return NULL;
}

watch_rule* watch_firewall_delete_rule(watch_firewall *this, watch_rule *rule)
{
	return NULL;
}

