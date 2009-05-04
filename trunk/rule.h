/*
 *  rule.h
 *  Watch
 *
 *  Created by Jan Bird on 3/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef WATCH_RULE_H
#define WATCH_RULE_H


#include <sys/param.h>
#include <sys/socket.h>


typedef struct _watch_rule
{
	size_t size;//this + process name + file path + sockaddress 
	u_int32_t id;

	u_int16_t process_name_size;
	u_int16_t process_name_offset;//0 for all
	u_int16_t file_path_size;
	u_int16_t file_path_offset;//0 for all

	u_int16_t sock_domain;//0 for all
	u_int16_t sock_type;//0 for all
	u_int16_t sock_protocol;// 0 fro all	
	u_int16_t sockadress_offset;// 0 for all

	u_int8_t direction;//0 both. 1 incoming, 2 outgoung
	u_int8_t allow;//0 denny, 1 allow

	//char* process_name;
	//char* file_path;
	//sockaddr 
	
	
} watch_rule;

typedef struct _ask_watch_rule
{
	size_t size;
	watch_rule rule;
	pid_t pid;
	uid_t uid;
} watch_ask_rule;


watch_rule* init_rule(watch_rule* this, 
					  u_int32_t id, char* process_name, size_t process_name_size, 
					  char* file_path, 
					  size_t file_path_size, u_int16_t sock_domain, u_int16_t sock_type, 
					  u_int16_t sock_protocol, struct sockaddr* sockadress, size_t sockaddress_size, 
					  u_int8_t direction, u_int8_t allow);

int compare_rule(watch_rule *left, watch_rule *rigth);

#endif