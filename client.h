/*
 *  client.h
 *  Watch
 *
 *  Created by JanBird on 5/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <libkern/OSTypes.h>
#include <libkern/c++/OSObject.h>
#include <sys/kern_control.h>

#include "messages.h"


enum ClientState
{
	ClientStateActive = 1,
	ClientStateClosed = 2
};

class Client: public OSObject
{
	OSDeclareDefaultStructors(Client)
	
public:
	kern_ctl_ref *ptr_kernelKontrolReference;
	UInt32 unit;
	ClientState state;
	
	bool initWithClient(kern_ctl_ref* ptr_kernelKontrolReference, UInt32 unit, ClientState clientState);
	
	virtual void free();
	
	void Send(Message* message);

};

class ClientNode: public OSObject
{
	OSDeclareDefaultStructors(ClientNode)

public:
	Client *client;
	ClientNode *next;
	
	bool init(Client* client, ClientNode *node);
	
	virtual void free();
};