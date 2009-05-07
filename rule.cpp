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

OSDefineMetaClassAndStructors(Rule, OSObject)

bool Rule::init(
				u_int32_t id, 
				char* process_name, 
				char* file_path, 
				u_int16_t sock_domain, 
				u_int16_t sock_type, 
				u_int16_t sock_protocol, 
				struct sockaddr* sockadress, 
				u_int8_t direction, 
				u_int8_t allow)
{	
	//clear memory
	this->id = id;
	if(process_name && !(this->processName = OSString::withCString(process_name)))
		return false;
	
	if(file_path && !(this->filePath = OSString::withCString(file_path)))
	{
		if(this->filePath) this->filePath->release();
		return false;
	}
	
	this->sock_domain = sock_domain;
	this->sock_type = sock_type;
	this->sock_protocol = sock_protocol;
	
	
	this->direction = direction;
	this->allow = allow;
	
	return true;
}

void Rule::free()
{
	if(this->filePath)
		this->filePath->release();
	
	if(this->processName)
		this->processName->release();
	
	OSObject::free();
}

int compare_rule(Rule *left, Rule *rigth)
{
	//if(left->)
	return 0;
}
