#include <string.h>

#include "rule.h"

bool 
Rule::init(
				UInt32 id, 
				char* processName, 
				char* filePath, 
				UInt16 sockDomain, 
				UInt16 sockType, 
				UInt16 sockProtocol, 
				struct sockaddr* sockadress, 
				UInt8 direction, 
				UInt8 allow)
{	
	this->id = id;
	if(processName && !(this->processName = OSString::withCString(processName)))
		return false;
	
	if(filePath && !(this->filePath = OSString::withCString(filePath)))
	{
		if(this->filePath) this->filePath->release();
		return false;
	}
	
	this->sockDomain = sockDomain;
	this->sockType = sockType;
	this->sockProtocol = sockProtocol;
	
	
	this->direction = direction;
	this->allow = allow;
	
	return true;
}

void 
Rule::free()
{
	if(this->filePath)
		this->filePath->release();
	
	if(this->processName)
		this->processName->release();
	
	SimpleBase::free();
}


bool 
Rules::isRulesChanged(time_t)
{
	return false;//TODO: refactor
}

Rule* 
Rules::findRule(const OSString* process_name, const OSString* process_path, 
			   UInt16 sock_famely, UInt16 socket_type, UInt16 sock_protocol, 
			   UInt8 direction, struct sockaddr *sockaddres )
{
	return NULL;
}

Rule* 
Rules::addRule(Rule *rule)
{
	IOLockLock(lock);
	Rule* c = this->root;
	while (c)
	{
		c = c->next;
	}
	
unlock:
	IOLockUnlock(lock);
	return NULL;//TODO: refactor
}

Rule* 
Rules::deleteRule(UInt32 ruleId)
{
	IOLockLock(lock);
	Rule* rule = this->root;
	while (rule)
	{
		if(rule->id == ruleId)
		{
			rule->state |= RuleStateDeleted;
			rule->removeFromChain();
			rule->release();
			goto unlock;
		}
	}
	
unlock:
	IOLockUnlock(lock);
	return NULL;//TODO: refactor
}

Rule* 
Rules::activateRule(UInt32 ruleId)
{
	IOLockLock(lock);
	Rule* rule = this->root;
	while (rule)
	{
		if(rule->id == ruleId)
		{
			rule->state |= RuleStateActive;
			goto unlock;
		}
	}
	
unlock:
	IOLockUnlock(lock);
	return NULL;//TODO: refactor
}

Rule* 
Rules::deactivateRule(UInt32 ruleId)
{
	IOLockLock(lock);
	Rule* rule = this->root;
	while (rule)
	{
		if(rule->id == ruleId)
		{
			rule->state |= ~RuleStateActive;
			goto unlock;
		}
	}
	
unlock:
	IOLockUnlock(lock);
	return NULL;//TODO: refactor
}

