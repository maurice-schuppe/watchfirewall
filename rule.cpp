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

//OSDefineMetaClassAndStructors(Rule, OSObject)

bool Rule::init(
				UInt32 id, 
				char* processName, 
				char* filePath, 
				UInt16 sockDomain, 
				UInt16 sockType, 
				UInt16 sockProtocol, 
				struct sockaddr* sockadress, 
				UInt8 direction, 
				UInt8 allow)
{	
	this->id = id;
	if(processName && !(this->processName = OSString::withCString(processName)))
		return false;
	
	if(filePath && !(this->filePath = OSString::withCString(filePath)))
	{
		if(this->filePath) this->filePath->release();
		return false;
	}
	
	this->sockDomain = sockDomain;
	this->sockType = sockType;
	this->sockProtocol = sockProtocol;
	
	
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
	
	SimpleBase::free();
}

int compare_rule(Rule *left, Rule *rigth)
{
	//if(left->)
	return 0;
}
