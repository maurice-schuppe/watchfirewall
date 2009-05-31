/*
 *  client.cpp
 *  Watch
 *
 *  Created by JanBird on 5/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "client.h"

//OSDefineMetaClassAndStructors(Client, OSObject)

void 
Client::ClearQueue(ClientMessageNode *root)
{
	while(root)
	{
		ClientMessageNode *curr = root;
		curr->message->release();
		root = root->next;
		delete(curr);
	}
}

bool 
Client::initWithClient(kern_ctl_ref kernelKontrolReference, UInt32 unit)
{
	::IOLog("client state refernces: %d; thread: %d; lQueue: %d; lThread: %d; nest: %d \n", this->references, this->thread, this->lockQueue, this->lockWorkThread, this->next);
	
	this->registredMessageClases = MessageClassFirewall | MessageClassCommon;
	
	if(this->lockQueue = IOSimpleLockAlloc())
	{
		if(this->lockWorkThread = IOLockAlloc())
		{
			this->kernelKontrolReference = kernelKontrolReference;
			this->unit = unit;
			
			if(this->thread = IOCreateThread(Client::SendThread, this))
			{
				::IOLog("client created \n");
				this->references = 1;
				return true;
			}
			
			IOLockFree(this->lockWorkThread);
			::IOLog("client can't create thread \n");//TODO: refactor
		}
		
		IOSimpleLockFree(this->lockQueue);
		::IOLog("client can't create lock thread \n");//TODO: refactor
	}
	
	::IOLog("client can't create lock client \n");//TODO: refactor
	return false;
}

void 
Client::closeSignal()
{
	::IOLog("cliend send close signal\n");
	OSIncrementAtomic(&this->exitState);
	IOLockWakeup(this->lockWorkThread, 0, false);//(this->lockWorkThread);
}

void
Client::free()
{
	//send exit thread
	::IOLog("client begin destroed\n");
	ClearQueue(this->messageQueueRoot);
	this->messageQueueRoot = NULL;
	this->messageQueueLast = NULL;
	
	if(this->lockQueue)
	{
		IOSimpleLockFree(this->lockQueue);
		this->lockQueue = NULL;
	}
	
	if(this->lockWorkThread)
	{
		IOLockFree(this->lockWorkThread);
		this->lockWorkThread = NULL;
	}
	
	SimpleBase::free();
	::IOLog("client destored\n");
}

void 
Client::Send(Message* message)
{
	if(message == NULL || this->exitState)
		return;
	
	if(!(message->m.type & this->registredMessageClases))
		return;
	
	ClientMessageNode * node = new ClientMessageNode();
	if(!node)
		return;

	message->retain();
	node->message = message;
	node->next = NULL;
	
	IOInterruptState istate = IOSimpleLockLockDisableInterrupt(this->lockQueue);
	
	if(this->messageQueueLast == NULL)
	{
		this->messageQueueLast = node;
		this->messageQueueRoot = node;
	}
	else
		this->messageQueueLast->next = node;
	
	IOSimpleLockUnlockEnableInterrupt(this->lockQueue, istate);
	IOLockWakeup(this->lockWorkThread, 0, false);
}

void 
Client::SendThread(void* arg)
{
	Client* client = (Client*)arg;
	client->retain();
	ClientMessageNode *node = NULL;
	IOLockLock(client->lockWorkThread);

	while(1)
	{
		IOLockSleep(client->lockWorkThread, 0, THREAD_UNINT);
		
		if(client->exitState)
			goto exit;
		
		IOInterruptState istate = IOSimpleLockLockDisableInterrupt(client->lockQueue);

		node = client->messageQueueRoot;
		client->messageQueueRoot = NULL;
		client->messageQueueLast = NULL;
		
		IOSimpleLockUnlockEnableInterrupt(client->lockQueue, istate);
		
		while(node)
		{
			if(client->exitState)
				goto exitAndClearQueue;
			
			int result;
			while(result = ctl_enqueuedata(client->kernelKontrolReference, client->unit, &(node->message->m), node->message->m.size, 0))
			{
				if(client->exitState)
					goto exitAndClearQueue;

				switch(result)
				{
					case EINVAL: // - Invalid parameters.
						::IOLog("ctl_enqueuedata return: Invalid parameter.\n");
						goto next;
						break;
					case EMSGSIZE: // - The buffer is too large.
						::IOLog("ctl_enqueuedata return: The buffer is too large.\n");
						goto next;
						break;
					case ENOBUFS: // - The queue is full or there are no free mbufs.
						::IOLog("ctl_enqueuedata return: The queue is full or there are no free mbufs.\n");
						IOSleep(100);
						break;
				}
				
				if(client->exitState)
					goto exitAndClearQueue;
			}
			
		next:

			ClientMessageNode *deletedNode = node;
			node = node->next;
			deletedNode->message->release();
			delete(deletedNode);
		}
	}
	
exitAndClearQueue:
	//::IOLog("client in exit and clear queue \n");
	ClearQueue(node);
exit:
	//::IOLog("client in exit\n");
	IOLockUnlock(client->lockWorkThread);
	client->release();
	
	//::IOLog("work thread exit \n");

	IOExitThread();
}

void 
Client::registerMessageClasses(UInt16 classes)
{
	OSBitOrAtomic(classes, &this->registredMessageClases);	
}

void 
Client::unregisterMessageClasses(UInt16 classes)
{
	OSBitOrAtomic(~classes, &this->registredMessageClases);	
}	



