#include <string.h>
#include <kern/clock.h>

#include "rule.h"

bool 
Rule::init(MessageAddRule *message)
{	
	if(message)
	{
		if(this->processName = OSString::withCString(message->getProcessName()))
		{
			if(this->filePath = OSString::withCString(message->getFilePath()))
			{
				sockaddr *t = message->getSockAddress();
				if(t)
				{
					this->sockAddress = (sockaddr*)new char[t->sa_len];
					if(this->sockAddress)
					{
						memcpy(this->sockAddress, t, t->sa_len);
					}
				}

				if((t && this->sockAddress) || !t)
				{
					this->id = message->id;
					
					this->sockDomain = message->sockDomain;
					this->sockType = message->sockType;
					this->sockProtocol = message->sockProtocol;
					
					
					this->direction = message->direction;
					this->allow = message->allow;
					this->state = message->state;
					this->references = 1;
					
					return true;
				}
				this->filePath->release();
			}
			this->processName->release();
		}
	}
	
	return false;
}

void 
Rule::free()
{
	if(this->filePath)
		this->filePath->release();
	
	if(this->processName)
		this->processName->release();
	
	if(this->sockAddress)
		delete sockAddress;
	
	SimpleBase::free();
}

int 
Rule::compare(Rule *toRule)
{
	int result = 0;
	result = strcmp(this->processName->getCStringNoCopy(), toRule->processName->getCStringNoCopy());
	if(result != 0)
		return result;
	
	result = strcmp(this->filePath->getCStringNoCopy(), toRule->filePath->getCStringNoCopy());
	if(result != 0)
		return result;
	
	result = (int)this->sockDomain - toRule->sockDomain;
	if(result != 0)
		return result;
	
	result = (int)this->sockType - toRule->sockType;
	if(result != 0)
		return result;
	
	result = (int)this->sockProtocol - toRule->sockProtocol;
	if(result != 0)
		return result;
	
	//get sock address comparrer
	
	result = (int)this->direction - toRule->direction;
	if(result != 0)
		return result;
	
	result = (int)this->allow - toRule->allow;
	
	return result;
}

Rule* 
Rules::findRule(const OSString* processName, const OSString* filePath, 
			   UInt16 sockDomain, UInt16 sockType, UInt16 sockProtocol, 
			   UInt8 direction, struct sockaddr *sockaddres )
{
	IOLockLock(lock);
	Rule* current = this->root;
	//bool beIdentical=true;
	while (current)
	{
		if(current->processName->getLength() == 0 || current->processName->isEqualTo(processName))
		{
			if(current->filePath->getLength() == 0 || current->filePath->isEqualTo(filePath))
			{
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
		}
		current = current->next;
	}
	
unlock:
	IOLockUnlock(lock);
	return current;
}

Rule* 
Rules::addRule(MessageAddRule *messageRule)
{
	Rule *rule = new Rule();
	if(rule == NULL)
	{
		IOLog("can't create Rule");
		return NULL;
	}
	
	if(rule->init(messageRule) == false)
	{
		delete rule;
		return NULL;
	}
	
	IOLockLock(lock);
	if(this->root == NULL)
	{
		this->root = rule;
	}
	else
	{
		Rule* c = this->root;
		int result;
		while (c)
		{
			result = rule->compare(c);
			if(result == 0)
			{
				rule->release();
				rule = c;
				break;//rule exist
			}
			if(result > 0)
			{
				//insert before c
				rule->next = c;
				c->prev = rule;
				
				rule->prev = c->prev;
				if(rule->prev)
					rule->prev = rule;
				else
					this->root = rule;//prev is root, replace
				
				//get rule id
				
				clock_get_uptime(&lastChangedTime);
				break;
			}
		}
	}
	
unlock:
	IOLockUnlock(lock);
	return rule;//TODO: refactor
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
			
			clock_get_uptime(&lastChangedTime);
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
			if((rule->state & RuleStateActive) == 0)
			{
				rule->state |= RuleStateActive;
				clock_get_uptime(&lastChangedTime);
			}
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
			if((rule->state & RuleStateActive) > 0)
			{
				rule->state |= ~RuleStateActive;
				clock_get_uptime(&lastChangedTime);
			}
			goto unlock;
		}
	}
	
unlock:
	IOLockUnlock(lock);
	return NULL;//TODO: refactor
}

