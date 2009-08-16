#include <IOKit/IOLib.h>
#include "firewall.h"

#pragma mark kext routine

extern "C" {

kern_return_t 
watch_start (kmod_info_t * ki, void * d)
{		
	return Firewall::InitGlobal() ? KERN_SUCCESS : KERN_FAILURE;
}


kern_return_t 
watch_stop (kmod_info_t * ki, void * d) 
{
	return Firewall::FreeGlobal() ? KERN_SUCCESS : EBUSY;
}

}