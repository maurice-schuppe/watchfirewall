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

struct RuleSimple
{
	size_t size;//this + process name + file path + sockaddress 
	u_int32_t id;
	
	u_int16_t process_name_size;
	u_int16_t process_name_offset;//0 for all
	u_int16_t file_path_size;
	u_int16_t file_path_offset;//0 for all
	
	u_int16_t sock_domain;//0 for all
	u_int16_t sock_type;//0 for all
	u_int16_t sock_protocol;// 0 fro all	
	u_int16_t sockadress_offset;// 0 for all
	
	u_int8_t direction;//0 both. 1 incoming, 2 outgoung
	u_int8_t allow;//0 denny, 1 allow
};

class Rule: public OSObject
{
	OSDeclareDefaultStructors(Rule)
	
	UInt32 id;
	
	OSString *processName;
	OSString *filePath;
	
	UInt16 sock_domain;//0 for all
	UInt16 sock_type;//0 for all
	UInt16 sock_protocol;// 0 fro all	
	UInt16 sockadress_offset;// 0 for all
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	UInt8 allow;//0 denny, 1 allow
	
	UInt16 references;
	UInt8 state;
	
	IOLock *lock;

public:
	
	bool init(u_int32_t id, char* process_name, char* file_path, 
						  UInt16 sock_domain, UInt16 sock_type, 
						  UInt16 sock_protocol, struct sockaddr* sockadress, 
						  UInt8 direction, UInt8 allow);
	virtual void free();
	
	bool isApplicable();
};

struct RuleNode
{
	Rule *rule;
	RuleNode *next;
};

struct AskRule
{
	size_t size;
	RuleSimple rule;
	pid_t pid;
	uid_t uid;
};


int compare_rule(Rule *left, Rule *rigth);

#endif