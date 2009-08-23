#include "cookie.h"
#include <sys/proc.h>

//IOLock *SocketCookie::lock;
//SocketCookie *SocketCookie::socketCookies;


//used only if exit extension
bool
SocketCookie::IsValid()
{
	//check socket
	
	//check last pid used
	proc_t p;
	p = proc_find(this->application->pid);
	if (p)
	{
		proc_rele(p);
		return true;
	}
	return false;
}

bool 
SocketCookie::SetFromAddress(const sockaddr *socketAddress)
{
	if(this->fromAddress)
	{
		delete this->fromAddress;
		this->fromAddress = NULL;
	}
	
	this->fromAddress = (sockaddr*)new UInt8[socketAddress->sa_len];
	if(this->fromAddress == NULL)
		return false;
	
	memcpy(this->fromAddress, socketAddress, socketAddress->sa_len);
	
	return true;
}

bool 
SocketCookie::SetToAddress(const sockaddr *socketAddress)
{
	if(this->toAddress)
	{
		delete this->toAddress;
		this->toAddress = NULL;
	}
	
	this->toAddress = (sockaddr*)new UInt8[socketAddress->sa_len];
	if(this->toAddress == NULL)
		return false;
	
	memcpy(this->toAddress, socketAddress, socketAddress->sa_len);
	
	return true;
	
}


bool 
SocketCookie::AddDeferredData(bool direction,	mbuf_t data, mbuf_t control, sflt_data_flag_t flags, sockaddr *socketAddress)
{
	DeferredData *deferredData = new DeferredData;
	if(deferredData)
	{
		deferredData->data = data;
		deferredData->control = control;
		deferredData->flags = flags;
		//TODO: by size
		deferredData->socketAddress = socketAddress;
		
		if(lastDefferedData)
		{
			lastDefferedData->next = deferredData;
			lastDefferedData = deferredData;
		}
		
	}
	return false;
}

bool 
SocketCookie::ClearDeferredData()
{
	return false;
}

bool 
SocketCookie::SendDeferredData()
{
	return false;
}

