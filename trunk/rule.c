/*
 *  rule.c
 *  Watch
 *
 *  Created by Jan Bird on 3/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <string.h>

#include "rule.h"

watch_rule* init_rule(watch_rule* this, 
					  u_int32_t id, 
					  char* process_name, 
					  size_t process_name_size, 
					  char* file_path, 
					  size_t file_path_size, 
					  u_int16_t sock_domain, 
					  u_int16_t sock_type, 
					  u_int16_t sock_protocol, 
					  struct sockaddr* sockadress, 
					  size_t sockaddress_size, 
					  u_int8_t direction, 
					  u_int8_t allow)
{	
	void* current_offset = (void*)this + sizeof(watch_rule);
	//clear memory
	this->size = sizeof(watch_rule) + process_name_size + file_path_size + sockaddress_size;
	this->id = id;
	
	this->process_name_size = process_name_size;
	if(process_name_size)
	{
		strncpy(current_offset, process_name, process_name_size);
		this->process_name_offset = current_offset - (void*)this; 
		current_offset += process_name_size;
	}
	
	this->file_path_size = file_path_size;
	if(file_path_size)
	{
		strncpy(current_offset, file_path, file_path_size);
		this->file_path_offset = current_offset - (void*)this;
		current_offset += file_path_size;
	}
	
	this->sock_domain = sock_domain;
	this->sock_type = sock_type;
	this->sock_protocol = sock_protocol;
	
	if(sockaddress_size)
	{
		memcpy(current_offset, sockadress, sockaddress_size);
		this->sockadress_offset = current_offset - (void*)this;
		current_offset += sockaddress_size;
	}
	
	this->direction = direction;
	this->allow = allow;
	
	return this;	
}
int compare_rule(watch_rule *left, watch_rule *rigth)
{
	//if(left->)
	return 0;
}
