/*
 *  rule.h
 *  Watch
 *
 *  Created by Jan Bird on 3/29/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef WATCH_RULE_H
#define WATCH_RULE_H


#include <sys/param.h>
#include <sys/socket.h>
#include <libkern/c++/OSString.h>
#include <IOKit/IOLib.h>

#include "simpleBase.h"

struct RuleSimple
{
	size_t size;//this + process name + file path + sockaddress 
	UInt32 id;
	
	UInt16 process_name_offset;//0 for all
	UInt16 file_path_offset;//0 for all
	
	UInt16 sock_domain;//0 for all
	UInt16 sock_type;//0 for all
	UInt16 sock_protocol;// 0 fro all	
	UInt16 sockadress_offset;// 0 for all
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	UInt8 allow;//0 denny, 1 allow
};

struct AskRule
{
	size_t size;
	RuleSimple rule;
	pid_t pid;
	uid_t uid;
};

enum RuleState
{
	RuleStateActive = 0,
	RuleStateDisabled = 1,
	RuleStateDeleted = 2
};

class Rule: public SimpleBase
{
	//OSDeclareDefaultStructors(Rule)
	
	UInt32 id;
	
	OSString *processName;
	OSString *filePath;
	
	UInt16 sockDomain;//0 for all
	UInt16 sockType;//0 for all
	UInt16 sockProtocol;// 0 fro all	
	UInt16 sockadressOffset;// 0 for all
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	UInt8 allow;//0 denny, 1 allow
	
	UInt8 state;
	
	IOLock *lock;

public:
	Rule* prev;
	Rule* next;
	
public:
	bool init(UInt32 id, char* process_name, char* filePath, 
						  UInt16 sockDomain, UInt16 sockType, 
						  UInt16 sockProtocol, struct sockaddr* sockadress, 
						  UInt8 direction, UInt8 allow);
	virtual void free();
	
	bool isApplicable();
	
	void removeFromChain()
	{
		if(prev)
			prev->next = next;
		
		if(next)
			next->prev = prev;
		
		prev = next = NULL;
	}
	
	friend class Rules;
};

class Rules 
{
public:
	Rule *root;
		
	time_t lastChangedTime; 
	IOLock *lock;
	//sorted by process_name , ...
	bool init()
	{ 
		lastChangedTime = 0;
		lock = IOLockAlloc();
		if(!lock)
			return false;
		
		return true; 
	}
	
	void free()
	{
		if(lock)
		{
			IOLockLock(lock);

			while(root)
			{
				Rule *r = root;
				root = root->next;
				r->removeFromChain();
				r->release();
			}
			IOLockUnlock(lock);
			
			IOLockFree(lock);
			lock = NULL;
		}
	}
	
	bool isRulesChanged(time_t);
	Rule* findRule(const OSString* process_name, const OSString* process_path, 
				   UInt16 sock_famely, UInt16 socket_type, UInt16 sock_protocol, 
				   UInt8 direction, struct sockaddr *sockaddres );
	
	
	Rule* addRule(Rule *rule);
	Rule* deleteRule(UInt32 ruleId);
	Rule* activateRule(UInt32 ruleId);
	Rule* deactivateRule(UInt32 ruleId);
	
};

#endif