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

int 
Rules::addRule(MessageAddRule *messageRule, Rule** rule)
{
	*rule = NULL;
	int result;
	Rule *workRule = new Rule();
	if(workRule == NULL)
	{
		IOLog("can't create Rule");
		return -1;
	}
	
	if(workRule->init(messageRule) == false)
	{
		delete workRule;
		return -1;
	}
	
	IOLockLock(lock);
	if(this->root == NULL)
	{
		this->root = workRule;
	}
	else
	{
		Rule* c = this->root;
		int result;
		while (c)
		{
			result = workRule->compare(c);
			if(result == 0)
			{
				workRule->release();
				c->retain();
				*rule = c;
				result = 1;//rule exist
				break;
			}
			if(result > 0)
			{
				//insert before c
				workRule->next = c;
				workRule->prev = c->prev;

				c->prev = workRule;
				
				if(workRule->prev)
					workRule->prev->next = workRule;
				else
					this->root = workRule;//prev is root, replace
				
				//get rule id
				workRule->retain();
				*rule = workRule;
				result = 0;
				
				clock_get_uptime(&lastChangedTime);
				break;
			}
		}
	}
	
unlock:
	IOLockUnlock(lock);
	return result;
}

int 
Rules::deleteRule(UInt32 ruleId, Rule** rule)
{
	*rule = NULL;
	IOLockLock(lock);
	Rule* workRule = this->root;
	while (workRule)
	{
		if(workRule->id == ruleId)
		{
			workRule->state |= RuleStateDeleted;
			removeFromChain(workRule);
			
			clock_get_uptime(&lastChangedTime);
			*rule = workRule;
			break;
		}
		
		workRule = workRule->next;
	}
	
	IOLockUnlock(lock);
	return (*rule != NULL) ? 1 : 0;
}

int 
Rules::activateRule(UInt32 ruleId, Rule** rule)
{
	*rule = NULL;
	int result = -1;
	IOLockLock(lock);
	Rule* workRule = this->root;
	while (workRule)
	{
		if(workRule->id == ruleId)
		{
			if((workRule->state & RuleStateActive) == 0)
			{
				workRule->state |= RuleStateActive;
				clock_get_uptime(&lastChangedTime);
				result = 0;
			}
			else
			{
				result = 1;
			}

			workRule->retain();
			*rule = workRule;
			break;
		}
		
		workRule = workRule->next;
	}
	
	IOLockUnlock(lock);
	return result;
}

int 
Rules::deactivateRule(UInt32 ruleId, Rule** rule)
{
	int result = -1;
	*rule = NULL;
	IOLockLock(lock);
	Rule* workRule = this->root;
	while (workRule)
	{
		if(workRule->id == ruleId)
		{
			if((workRule->state & RuleStateActive) > 0)
			{
				workRule->state |= ~RuleStateActive;
				clock_get_uptime(&lastChangedTime);
				result = 0;
			}
			else
			{
				result = 1;//alredy inactive
			}

			workRule->retain();
			*rule = workRule;
			break;
		}
		
		workRule = workRule->next;
	}

	IOLockUnlock(lock);
	return result;
}

