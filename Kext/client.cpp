#include "client.h"

void 
Client::ClearQueue(ClientMessageNode *root)
{
	while(root)
	{
		ClientMessageNode *curr = root;
		curr->message->Release();
		root = root->next;
		delete(curr);
	}
}

bool 
Client::InitWithClient(kern_ctl_ref kernelKontrolReference, UInt32 unit)
{
	IOLog("client state refernces: %ld; thread: %p; lQueue: %p; lThread: %p; nest: %p \n", this->references, this->thread, this->lockQueue, this->lockWorkThread, this->next);
	
	this->registredMessageClases = MessageClassFirewall | MessageClassCommon;
	
	if(this->lockQueue = IOSimpleLockAlloc())
	{
		if(this->lockWorkThread = IOLockAlloc())
		{
			this->kernelKontrolReference = kernelKontrolReference;
			this->unit = unit;
			
			if(this->thread = IOCreateThread(Client::SendThread, this))
			{
				IOLog("client created \n");
				this->references = 1;
				return true;
			}
			
			IOLockFree(this->lockWorkThread);
			IOLog("client can't create thread \n");//TODO: refactor
		}
		
		IOSimpleLockFree(this->lockQueue);
		IOLog("client can't create lock thread \n");//TODO: refactor
	}
	
	IOLog("client can't create lock client \n");//TODO: refactor
	return false;
}

void 
Client::CloseSignal()
{
	IOLog("cliend send close signal\n");
	OSIncrementAtomic(&this->exitState);
	IOLockWakeup(this->lockWorkThread, 0, false);
}

void
Client::Free()
{
	//send exit thread
	IOLog("client begin destroed\n");
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
	
	SimpleBase::Free();
	::IOLog("client destored\n");
}

void 
Client::Send(Message* message)
{
	if(message == NULL || this->exitState)
		return;
	
	if(!(message->GetRawMessageType() & this->registredMessageClases))
		return;
	
	ClientMessageNode * node = new ClientMessageNode();
	if(!node)
		return;

	message->Retain();
	node->message = message;
	node->next = NULL;
	
	IOInterruptState istate = IOSimpleLockLockDisableInterrupt(this->lockQueue);
	
	if(this->messageQueueLast == NULL)
		this->messageQueueRoot = node;
	else
		this->messageQueueLast->next = node;
	
	this->messageQueueLast = node;

	IOSimpleLockUnlockEnableInterrupt(this->lockQueue, istate);
	IOLockWakeup(this->lockWorkThread, 0, false);
}

void 
Client::SendThread(void* arg)
{
	Client* client = (Client*)arg;
	client->Retain();
	ClientMessageNode *node = NULL;
	UInt64 lastSendTime = 0;
	UInt64 currentTime;
	UInt64 diff;
	size_t size;
	int k;

	IOLockLock(client->lockWorkThread);

	while(1)
	{
		IOLockSleep(client->lockWorkThread, 0, THREAD_UNINT);
		
		if(client->exitState)
			goto exit;
		
		//sllep if nedded
		clock_get_uptime(&currentTime);
		absolutetime_to_nanoseconds( currentTime - lastSendTime, &diff);
		
		if(diff < 500000000)//nano seconds
			IOSleep(500);
		
		IOInterruptState istate = IOSimpleLockLockDisableInterrupt(client->lockQueue);

		node = client->messageQueueRoot;
		client->messageQueueRoot = NULL;
		client->messageQueueLast = NULL;
		
		IOSimpleLockUnlockEnableInterrupt(client->lockQueue, istate);
		
		while(node)
		{
			size = 0;
			k = 0;
			do
			{
				if(client->exitState)
					goto exitAndClearQueue;

				ctl_getenqueuespace(client->kernelKontrolReference, client->unit, &size);
				if(size < node->message->GetRawMessageSize())
				{
					if(k++ < 3)
					{
						IOSleep(200);
						continue;
					}
					else
						::IOLog("client is bisy. can't recivie data.\n");
				}
				else
				{
					if(client->exitState)
						goto exitAndClearQueue;
					
					switch(ctl_enqueuedata(client->kernelKontrolReference, client->unit, node->message->GetRawMessage(), node->message->GetRawMessageSize(), 0))
					{
						case EINVAL: // - Invalid parameters.
							::IOLog("ctl_enqueuedata return: Invalid parameter.\n");
							break;
						case EMSGSIZE: // - The buffer is too large.
							::IOLog("ctl_enqueuedata return: The buffer is too large.\n");
							break;
						case ENOBUFS: // - The queue is full or there are no free mbufs.
							::IOLog("ctl_enqueuedata return: The queue is full or there are no free mbufs.\n");
							break;
					}
				}				
			}while(false);

			
			if(client->exitState)
				goto exitAndClearQueue;

			ClientMessageNode *deletedNode = node;
			node = node->next;
			deletedNode->message->Release();
			delete(deletedNode);
		}
		
		clock_get_uptime(&lastSendTime);
	}
	
exitAndClearQueue:
	ClearQueue(node);
exit:
	IOLockUnlock(client->lockWorkThread);
	client->Release();

	IOExitThread();
}

bool 
Client::RegisterMessageClasses(UInt16 classes)
{
	if(this->registredMessageClases & classes == classes)
		return false;
	
	OSBitOrAtomic(classes, &this->registredMessageClases);
	return true;
}

bool 
Client::UnregisterMessageClasses(UInt16 classes)
{
	if(this->registredMessageClases & classes == 0)
		return false;

	OSBitOrAtomic(~classes, &this->registredMessageClases);
	return true;
}	



