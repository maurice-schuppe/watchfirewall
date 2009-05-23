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
Rules::findRule(const OSString* processName, const OSString* processPath, 
			   UInt16 sockDomain, UInt16 sockType, UInt16 sockProtocol, 
			   UInt8 direction, struct sockaddr *sockaddres )
{
	IOLockLock(lock);
	Rule* current = this->root;
	//bool beIdentical=true;
	while (current)
	{
		if(current->processName == NULL || current->processName->getLength() == 0 || current->processName->isEqualTo(processName))
		{
			//skip process path
			if(current->sockDomain == 0 || current->sockDomain == sockDomain)
			{
				if(current->sockType == 0 || current->sockType == sockType)
				{
					if(current->sockProtocol == 0 || current->sockProtocol == sockProtocol)
					{
						if((current->direction & direction) > 0)
						{
							//skip sockaddress
							
							current->retain();
							break;
						}
						
					}
				}
			}
		}
		current = current->next;
	}
	
unlock:
	IOLockUnlock(lock);
	return current;
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
			//check is root
			if(this->root == rule)
				this->root = NULL;
			
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

