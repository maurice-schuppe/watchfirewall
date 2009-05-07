/*
 *  client.cpp
 *  Watch
 *
 *  Created by JanBird on 5/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <IOKit/IOLib.h>

#include "client.h"

OSDefineMetaClassAndStructors(Client, OSObject)

bool 
Client::initWithClient(kern_ctl_ref* ptr_kernelKontrolReference, UInt32 unit, ClientState clientState)
{
	this->ptr_kernelKontrolReference = ptr_kernelKontrolReference;
	this->unit = unit;
	this->state = clientState;
	return true;
}

void
Client::free()
{
	OSObject::free();
}

void 
Client::Send(Message* message)
{
	//Send threaded
	
	if(!(*this->ptr_kernelKontrolReference))
		return;
	
	size_t space = 0;

	do{
		if(!ctl_getenqueuespace(*this->ptr_kernelKontrolReference, this->unit, &space))
		{
			if(space >= message->length)
				break;
			
			IOSleep(50);
		}
	
	}while(space < message->length);

	ctl_enqueuedata(*this->ptr_kernelKontrolReference, this->unit, message, message->length, 0);

}

OSDefineMetaClassAndStructors(ClientNode, OSObject)

void
ClientNode::free()
{
	if(this->client) this->client->release();
	OSObject::free();
}


bool 
ClientNode::init(Client* client, ClientNode *node)
{
	this->client = client;
	this->client->retain();
	this->next = node;
	return true;
}