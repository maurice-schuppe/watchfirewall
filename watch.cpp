#include <IOKit/IOLib.h>
#include "firewall.h"

#pragma mark kext routine

extern "C" {

kern_return_t 
watch_start (kmod_info_t * ki, void * d)
{		
	if(!Firewall::Init())
		return KERN_FAILURE;

	return KERN_SUCCESS;
}


kern_return_t 
watch_stop (kmod_info_t * ki, void * d) 
{
	//IOLog("in watch_stop \n");
	if(!Firewall::Free())
		return EBUSY;
	
	//IOLog("stoped close return true \n");
	return KERN_SUCCESS;
}

}