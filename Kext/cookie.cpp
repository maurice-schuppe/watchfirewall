#include "cookie.h"
#include <sys/proc.h>

mbuf_tag_id_t SocketCookies::mbufTagId;

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
SocketCookie::AddDeferredData(bool direction, mbuf_t data, mbuf_t control, sflt_data_flag_t flags, sockaddr *socketAddress)
{
	DeferredData *deferredData = new(socketAddress->sa_len) DeferredData;
	if(deferredData)
	{
		deferredData->data = data;
		deferredData->control = control;
		deferredData->flags = flags;
		//TODO: by size
		memcpy(&deferredData->socketAddress, socketAddress, socketAddress->sa_len);
		
		if(lastDefferedData)
		{
			lastDefferedData->next = deferredData;
			lastDefferedData = deferredData;
		}
		else
		{
			rootDefferedData = deferredData;
			lastDefferedData = rootDefferedData;
		}
		
		return true;
		
	}
	return false;
}

bool 
SocketCookie::ClearDeferredData()
{
	while (rootDefferedData) 
	{
		mbuf_freem(rootDefferedData->data);
		mbuf_freem(rootDefferedData->control);
		
		rootDefferedData = rootDefferedData->next;
	}
	
	lastDefferedData = NULL;
	return true;
}

bool 
SocketCookie::SendDeferredData(bool isUnboundConnection)
{
	while (rootDefferedData) 
	{
		sockaddr *sockAddress = NULL;
		if(isUnboundConnection)
			sockAddress = (sockaddr*)&rootDefferedData->socketAddress;
		
		
		if(rootDefferedData->direction)
		{
			//TODO: socket address if tcp/ip
			if(sock_inject_data_in(socket, sockAddress, rootDefferedData->data, rootDefferedData->control, rootDefferedData->flags))
			{
				mbuf_freem(rootDefferedData->data);
				mbuf_freem(rootDefferedData->control);
			}
		}
		else
		{
			sock_inject_data_out(socket, sockAddress, rootDefferedData->data, rootDefferedData->control, rootDefferedData->flags);
		}
		
		rootDefferedData = rootDefferedData->next;
	}
	
	lastDefferedData = NULL;
	return true;
}

