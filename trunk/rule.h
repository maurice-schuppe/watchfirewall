#ifndef WATCH_RULE_H
#define WATCH_RULE_H


#include <sys/param.h>
#include <sys/socket.h>
#include <libkern/c++/OSString.h>
#include <IOKit/IOLib.h>

#include "bundleid.h"
#include "simpleBase.h"
#include "MessageType.h"


enum __attribute__((visibility("hidden"))) RuleState
{
	RuleStateActive = 1,
	RuleStateDeleted = 2
};

class __attribute__((visibility("hidden"))) Rule: public SimpleBase
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

private:
	Rule* prev;
	Rule* next;
	
public:
	bool Init(RawMessageAddRule *message);
	virtual void Free();
	
	bool IsApplicable();
	
	int Compare(Rule *toRule);
	
	friend class Rules;
};

class __attribute__((visibility("hidden"))) Rules 
{
public:
	Rule *root;
		
	UInt64 lastChangedTime; 
	IOLock *lock;
	//sorted by process_name , ...
	bool Init()
	{ 
		lastChangedTime = 0;
		lock = IOLockAlloc();
		if(!lock)
			return false;
		
		return true; 
	}
	
	void Free()
	{
		if(lock)
		{
			IOLockLock(lock);

			while(root)
				RemoveFromChain(root)->Release();

			IOLockUnlock(lock);
			
			IOLockFree(lock);
			lock = NULL;
		}
	}
	
	Rule* RemoveFromChain(Rule *rule)
	{
		if(rule->prev)
			rule->prev->next = rule->next;
		
		if(rule == root)
			root = rule->next;
		
		if(rule->next)
			rule->next->prev = rule->prev;
		
		rule->prev = rule->next = NULL;
		
		return rule;
		
	}
	bool IsRulesChanged(UInt64 fromTime) { return lastChangedTime > fromTime;}
	Rule* FindRule(const OSString* process_name, const OSString* process_path, 
				   UInt16 sock_famely, UInt16 socket_type, UInt16 sock_protocol, 
				   UInt8 direction, struct sockaddr *sockaddres );
	
	
	int AddRule(RawMessageAddRule *messageRule, Rule** rule);
	int DeleteRule(UInt32 ruleId, Rule** rule);
	int ActivateRule(UInt32 ruleId, Rule** rule);
	int DeactivateRule(UInt32 ruleId, Rule** rule);
	
};

#endif