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
	
	if(!(message->getType() & this->registredMessageClases))
		return;
	::IOLog("send message to client: %d\n", this->unit);
	//message->IOLog();
	
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
	//IOLockLock(this->lockWorkThread);
	IOLockWakeup(this->lockWorkThread, 0, false);
}

void 
Client::SendThread(void* arg)
{
	Client* client = (Client*)arg;
	client->retain();
	size_t space = 0;
	ClientMessageNode *node = NULL;
	IOLockLock(client->lockWorkThread);
	
	//::IOLog("in client thread \n");
	
	//int state = 0;
	while(1)
	{
		::IOLog("work thread go to sleep \n");

		IOLockSleep(client->lockWorkThread, 0, THREAD_UNINT);
		::IOLog("work thread weakup \n");
		
		if(client->exitState)
			goto exit;
		
		IOInterruptState istate = IOSimpleLockLockDisableInterrupt(client->lockQueue);
		//get first node
		node = client->messageQueueRoot;
		client->messageQueueRoot = NULL;
		client->messageQueueLast = NULL;
		
		IOSimpleLockUnlockEnableInterrupt(client->lockQueue, istate);
		
		while(node)
		{
//				::IOLog("in thread send message");
//				node->message->IOLog();
			for(UInt32 count = 4; count; count--)
			{
				if(client->exitState)
					goto exitAndClearQueue;
				
				if(!ctl_getenqueuespace(client->kernelKontrolReference, client->unit, &space))
				{
					if(space >= node->message->getLength())
					{
						if(client->exitState)
							goto exitAndClearQueue;
						
						ctl_enqueuedata(client->kernelKontrolReference, client->unit, node->message->getBuffer(), node->message->getLength(), 0);
						::IOLog("message sended \n");
						break;
					}

					if(client->exitState)
						goto exitAndClearQueue;
					
					IOSleep(50);
					
					if(client->exitState)
						goto exitAndClearQueue;
				}
			}
			
			ClientMessageNode *deleteNode = node;
			node = node->next;
			deleteNode->message->release();
			delete(deleteNode);//???
		}
	}
	
exitAndClearQueue:
	::IOLog("client in exit nad clear quque \n");
	ClearQueue(node);
exit:
	::IOLog("client in exit\n");
	IOLockUnlock(client->lockWorkThread);
	client->release();
	
	//IOSleep(2000);
	::IOLog("work thread exit \n");

	IOExitThread();
}



