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
				UInt32 id, 
				char* process_name, 
				char* file_path, 
				UInt16 sock_domain, 
				UInt16 sock_type, 
				UInt16 sock_protocol, 
				struct sockaddr* sockadress, 
				UInt8 direction, 
				UInt8 allow)
{	
	//clear memory
	if(!OSObject::init())
		return false;
	
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
