/*
 *  client.cpp
 *  Watch
 *
 *  Created by JanBird on 5/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "client.h"

OSDefineMetaClassAndStructors(Client, OSObject)

void 
Client::ClearQueue(ClientMessageNode *root)
{
	while(root)
	{
		ClientMessageNode *curr = root;
		root = root->next;
		delete(curr);
	}
}

bool 
Client::initWithClient(kern_ctl_ref kernelKontrolReference, UInt32 unit)
{
	if(this->lockQueue = IOSimpleLockAlloc())
	{
		if(this->lockWorkThread = IOLockAlloc())
		{
			IOLockLock(this->lockWorkThread);
			this->kernelKontrolReference = kernelKontrolReference;
			this->unit = unit;
			
			if(this->thread == IOCreateThread(SendThread, this))
			{
				return true;
			}
			
			IOLockFree(this->lockWorkThread);
			IOLog("client can't create thread");//TODO: refactor
		}
		
		IOSimpleLockFree(this->lockQueue);
		IOLog("client can't create lock thread");//TODO: refactor
	}
	
	IOLog("client can't create lock client");//TODO: refactor
	return false;
}

void
Client::free()
{
	//send exit thread
	
	OSIncrementAtomic(&this->exitState);
	IOLockUnlock(this->lockWorkThread);
	
	ClearQueue(this->messageQueueRoot);
	
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
	
	OSObject::free();
}

void 
Client::Send(Message* message)
{
	if(message == NULL)
		return;
	
	ClientMessageNode * node = new ClientMessageNode();//???
	node->message = message;
	
	IOSimpleLockLock(this->lockQueue);
	
	if(this->messageQueueLast == NULL)
	{
		this->messageQueueLast = node;
		this->messageQueueRoot = node;
	}
	else
		this->messageQueueLast->next = node;
	
	IOSimpleLockUnlock(this->lockQueue);
	IOLockUnlock(this->lockWorkThread);
}

void 
Client::SendThread(void* arg)
{
	Client* client = (Client*)arg;
	size_t space = 0;
	ClientMessageNode *node = NULL;
	
	while(1)
	{
		IOLockLock(client->lockWorkThread);
		
		if(client->exitState)
			goto exit;
		
		while(1)
		{
			IOSimpleLockLock(client->lockQueue);
			//get first node
			node = client->messageQueueRoot;
			client->messageQueueRoot = NULL;
			client->messageQueueLast = NULL;
			
			IOSimpleLockUnlock(client->lockQueue);
			
			while(node)
			{
				for(UInt32 count = 4; count; count--)
				{
					if(client->exitState)
						goto exitAndClearQueue;
					
					if(!ctl_getenqueuespace(client->kernelKontrolReference, client->unit, &space))
					{
						if(space >= node->message->length)
						{
							if(client->exitState)
								goto exitAndClearQueue;
							
							ctl_enqueuedata(client->kernelKontrolReference, client->unit, node->message, node->message->length, 0);
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
				delete(deleteNode);//???
				
			}
		}
	}
	
exitAndClearQueue:
	ClearQueue(node);
exit:
	IOExitThread();
}



