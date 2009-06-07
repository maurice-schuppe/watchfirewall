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
#include <IOKit/IOLocks.h>
#include <IOKit/IOLib.h>

#include "messages.h"
#include "simpleBase.h"

struct ClientMessageNode 
{
	Message *message;
	ClientMessageNode *next;
};

class Client : public SimpleBase//: public OSObject
{
	//OSDeclareDefaultStructors(Client)

	
protected:
	static void ClearQueue(ClientMessageNode *root);
public:
	kern_ctl_ref kernelKontrolReference;
	UInt32 unit;
	
	volatile UInt32 registredMessageClases;
	
	volatile SInt32 exitState;
	IOSimpleLock *lockQueue;
	IOLock *lockWorkThread;
	IOThread thread;
	ClientMessageNode *messageQueueRoot;
	ClientMessageNode *messageQueueLast;
	
	Client* next;
	
	bool initWithClient(kern_ctl_ref kernelKontrolReference, UInt32 unit);
	
	bool registerMessageClasses(UInt16 classes);
	bool unregisterMessageClasses(UInt16 classes);
	
	virtual void free();
	void closeSignal();

	void Send(Message* message);
	static void SendThread(void* arg);

};

