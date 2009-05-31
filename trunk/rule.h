
#ifndef WATCH_RULE_H
#define WATCH_RULE_H


#include <sys/param.h>
#include <sys/socket.h>
#include <libkern/c++/OSString.h>
#include <IOKit/IOLib.h>

#include "simpleBase.h"
#include "MessageType.h"


enum RuleState
{
	RuleStateActive = 1,
	RuleStateDeleted = 2
};

class Rule: public SimpleBase
{
	UInt32 id;
	
	OSString *processName;
	OSString *filePath;
	
	UInt16 sockDomain;//0 for all
	UInt16 sockType;//0 for all
	UInt16 sockProtocol;// 0 fro all	
	sockaddr* sockAddress;// 0 for all
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	UInt8 allow;//0 denny, 1 allow
	
	UInt8 state;
	
	IOLock *lock;

public:
	Rule* prev;
	Rule* next;
	
public:
	bool init(MessageAddRule *message);
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
	
	int compare(Rule *toRule);
	
	friend class Rules;
};

class Rules 
{
public:
	Rule *root;
		
	UInt64 lastChangedTime; 
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
	
	bool isRulesChanged(UInt64 fromTime) { return lastChangedTime > fromTime;}
	Rule* findRule(const OSString* process_name, const OSString* process_path, 
				   UInt16 sock_famely, UInt16 socket_type, UInt16 sock_protocol, 
				   UInt8 direction, struct sockaddr *sockaddres );
	
	
	Rule* addRule(MessageAddRule *messageRule);
	Rule* deleteRule(UInt32 ruleId);
	Rule* activateRule(UInt32 ruleId);
	Rule* deactivateRule(UInt32 ruleId);
	
};

#endif