#ifndef WATCH_MESSAGES_TYPE_H
#define WATCH_MESSAGES_TYPE_H

#include <libkern/OSTypes.h>
#include <string.h>

enum MessagesClass
{
	MessageClassInfoRule	= 0x0100,
	MessageClassInfoSocket	= 0x0200,
	MessageClassAsk			= 0x0400,
	MessageClassFirewall	= 0x0800,
	MessageClassClient		= 0x1000,
	MessageClassCommon		= 0x2000
};

enum ServerMessagesType 
{
	MessageTypeText						= MessageClassCommon | 0x01, //dymmu
	
	MessageTypeSfltUnregistered			= MessageClassCommon | 0x02, //debug
	MessageTypeSfltAttach				= MessageClassCommon | 0x03, //debug
	MessageTypeSfltDetach				= MessageClassCommon | 0x04, //debug
	MessageTypeSfltNotify				= MessageClassCommon | 0x05, //debug
	MessageTypeSfltGetPeerName			= MessageClassCommon | 0x06, //debug
	MessageTypeSfltGetSockName			= MessageClassCommon | 0x07, //debug
	MessageTypeSfltDataIn				= MessageClassCommon | 0x08, //debug
	MessageTypeSfltDataOut				= MessageClassCommon | 0x09, //debug
	MessageTypeSfltConnectIn			= MessageClassCommon | 0x0A, //debug
	MessageTypeSfltConnectOut			= MessageClassCommon | 0x0B, //debug
	MessageTypeSfltBind					= MessageClassCommon | 0x0C, //debug
	MessageTypeSfltSetOption			= MessageClassCommon | 0x0D, //debug
	MessageTypeSfltGetOption			= MessageClassCommon | 0x0E, //debug
	MessageTypeSfltListen				= MessageClassCommon | 0x0F, //debug
	MessageTypeSfltIoctl				= MessageClassCommon | 0x10, //debug
	MessageTypeSfltAccept				= MessageClassCommon | 0x11, //debug
	
	MessageTypeAskRule					= MessageClassAsk |  0x01,
	
	MessageTypeRuleAdded				= MessageClassInfoRule | 0x01,
	MessageTypeRuleDeleted				= MessageClassInfoRule | 0x02,
	MessageTypeRuleDeactivated			= MessageClassInfoRule | 0x03,
	MessageTypeRuleActivated			= MessageClassInfoRule | 0x04,
	
	MessageTypeSocketData				= MessageClassInfoSocket | 0x07,
	MessageTypeSocketOpen				= MessageClassInfoSocket | 0x08,
	MessageTypeSocketClosed				= MessageClassInfoSocket | 0x09,
	
	MessageTypeFirewallActivated		= MessageClassFirewall | 0x01,
	MessageTypeFirewallDeactivated		= MessageClassFirewall | 0x02,
	
	MessageTypeRegistredForAsk			= MessageClassFirewall | 0x03,
	MessageTypeUnregistredAsk			= MessageClassFirewall | 0x04,
	MessageTypeRegistredForInfoRule		= MessageClassFirewall | 0x05,
	MessageTypeUnregistredInfoRule		= MessageClassFirewall | 0x06,
	MessageTypeRegistredForInfoSocket	= MessageClassFirewall | 0x07,
	MessageTypeUnregistredInfoSocket	= MessageClassFirewall | 0x08,

	MessageTypeFirewallClosing			= MessageClassFirewall | 0x09
};

enum ClientMessagesType
{
	MessageTypeAddRule					= MessageClassClient | 0x01,
	MessageTypeDeleteRule				= MessageClassClient | 0x02,
	MessageTypeActivateRule				= MessageClassClient | 0x03,
	MessageTypeDeactivateRule			= MessageClassClient | 0x04,

	MessageTypeActivateFirewall			= MessageClassClient | 0x05,
	MessageTypeDeactivateFirewall		= MessageClassClient | 0x06,

	MessageTypeRegisterForAsk			= MessageClassClient | 0x07,
	MessageTypeUnregisterAsk			= MessageClassClient | 0x08,
	MessageTypeRegisterForInfoRule		= MessageClassClient | 0x09,
	MessageTypeUnregisterInfoRule		= MessageClassClient | 0x0A,
	MessageTypeRegisterForInfoSocket	= MessageClassClient | 0x0B,
	MessageTypeUnregisterInfoSocket		= MessageClassClient | 0x0C
};

struct SockAddress
{
	UInt8 len;
	UInt8 family;
};

struct RawRule
{
	
};

struct RawMessageBase 
{
	UInt16 size;
	UInt16 type;
	
	inline void Init(UInt16 size, UInt16 type){this->size = size, this->type = type;}
	
	static const SockAddress dummySockAddress;
};

struct RawMessageSfltUnregistered : public RawMessageBase
{

	//	static void		Unregistered(sflt_handle handle);
	inline void Init()
	{
		RawMessageBase::Init(sizeof(RawMessageSfltUnregistered), MessageTypeSfltUnregistered);
	}
};

struct RawMessageSfltAttach : public RawMessageBase		
{
	//	static errno_t	Attach(void	**cookie, socket_t so);
	UInt64 so;
	
	inline void Init(UInt64 so)
	{
		RawMessageBase::Init(sizeof(RawMessageSfltAttach), MessageTypeSfltAttach);
		this->so = so;
	}
};

struct RawMessageSfltDetach : public RawMessageBase		
{
	//	static void		Detach(void	*cookie, socket_t so);
	UInt64 so;
	
	inline void Init(UInt64 so)
	{
		RawMessageBase::Init(sizeof(RawMessageSfltDetach), MessageTypeSfltDetach);
		this->so = so;
	}
};

struct RawMessageSfltNotify : public RawMessageBase		
{
	//	static void		Notify(void *cookie, socket_t so, sflt_event_t event, void *param);
	UInt64 so;
	UInt16 event;
	
	inline void Init(UInt64 so, UInt16 event)
	{
		RawMessageBase::Init(sizeof(RawMessageSfltNotify), MessageTypeSfltNotify);
		this->so = so;
		this->event = event;
	}
};

struct RawMessageSfltGetPeerName : public RawMessageBase	
{
	//	static int		GetPeerName(void *cookie, socket_t so, sockaddr **sa);
	UInt64 so;
	SockAddress sa;
	
	inline void Init(UInt16 size, UInt64 so , SockAddress *sa)
	{
		RawMessageBase::Init(size, MessageTypeSfltGetPeerName);
		this->so = so;
		if(sa)
			memcpy(&this->sa, sa, sa->len);
		else
			this->sa = dummySockAddress;
	}
	
	inline static UInt16 GetNeededSize(SockAddress *sa)
	{
		return sizeof(RawMessageSfltGetPeerName) + (sa) ? (sa->len - sizeof(SockAddress)) : 0; 
	}
};

struct RawMessageSfltGetSockName : public RawMessageBase	
{
	//	static int		GetSockName(void *cookie, socket_t so, sockaddr **sa);
	UInt64 so;
	SockAddress sa;
	
	inline void Init(UInt16 size, UInt64 so , SockAddress *sa)
	{
		RawMessageBase::Init(size, MessageTypeSfltGetSockName);
		this->so = so;
		if(sa)
			memcpy(&this->sa, sa, sa->len);
		else
			this->sa = dummySockAddress;
	}

	inline static UInt16 GetNeededSize(SockAddress *sa)
	{
		return sizeof(RawMessageSfltGetSockName) + (sa) ? (sa->len - sizeof(SockAddress)) : 0; 
	}
};

struct RawMessageSfltDataIn : public RawMessageBase		
{
	//	static errno_t	DataIn(void *cookie, socket_t so, const sockaddr *from, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags);
	UInt64 so;
	SockAddress from;
	
	inline void Init(UInt16 size, UInt64 so, SockAddress *from)
	{
		RawMessageBase::Init(size, MessageTypeSfltDataIn);
		
		this->so = so;
		if(from)
			memcpy(&this->from, from, from->len);
		else 
			this->from = dummySockAddress;
	}
	
	inline static UInt16 GetNeededSize(SockAddress *sa)
	{
		return sizeof(RawMessageSfltDataIn) + (sa) ? (sa->len - sizeof(SockAddress)) : 0; 
	}
};

struct RawMessageSfltDataOut : public RawMessageBase		
{
	//	static errno_t	DataOut(void *cookie, socket_t so, const sockaddr *to, mbuf_t *data, mbuf_t *control, sflt_data_flag_t flags);
	UInt64 so;
	SockAddress to;
	
	inline void Init(UInt16 size, UInt64 so, SockAddress *to)
	{
		RawMessageBase::Init(size, MessageTypeSfltDataOut);
		
		this->so = so;
		if(to)
			memcpy(&this->to, to, to->len);
		else 
			this->to = dummySockAddress;
	}

	inline static UInt16 GetNeededSize(SockAddress *sa)
	{
		return sizeof(RawMessageSfltDataOut) + (sa) ? (sa->len - sizeof(SockAddress)) : 0; 
	}
};

struct RawMessageSfltConnectIn : public RawMessageBase	
{
	//	static errno_t	ConnectIn(void *cookie, socket_t so, const sockaddr *from);
	UInt64 so;
	SockAddress from;
	
	inline void Init(UInt16 size, UInt64 so, SockAddress *from)
	{
		RawMessageBase::Init(size, MessageTypeSfltConnectIn);
		
		this->so = so;
		if(from)
			memcpy(&this->from, from, from->len);
		else 
			this->from = dummySockAddress;
	}

	inline static UInt16 GetNeededSize(SockAddress *sa)
	{
		return sizeof(RawMessageSfltConnectIn) + (sa) ? (sa->len - sizeof(SockAddress)) : 0; 
	}
};

struct RawMessageSfltConnectOut : public RawMessageBase	
{
	//	static errno_t	ConnectOut(void *cookie, socket_t so, const sockaddr *to);
	UInt64 so;
	SockAddress to;
	
	inline void Init(UInt16 size, UInt64 so, SockAddress *to)
	{
		RawMessageBase::Init(size, MessageTypeSfltConnectOut);
		
		this->so = so;
		if(to)
			memcpy(&this->to, to, to->len);
		else 
			this->to = dummySockAddress;
	}

	inline static UInt16 GetNeededSize(SockAddress *sa)
	{
		return sizeof(RawMessageSfltConnectOut) + (sa) ? (sa->len - sizeof(SockAddress)) : 0; 
	}
};

struct RawMessageSfltBind : public RawMessageBase			
{
	//	static errno_t	Bind(void *cookie, socket_t so, const sockaddr *to);
	UInt64 so;
	SockAddress to;
	
	inline void Init(UInt16 size, UInt64 so, SockAddress *to)
	{
		RawMessageBase::Init(size, MessageTypeSfltBind);
		
		this->so = so;
		if(to)
			memcpy(&this->to, to, to->len);
		else 
			this->to = dummySockAddress;
	}

	inline static UInt16 GetNeededSize(SockAddress *sa)
	{
		return sizeof(RawMessageSfltBind) + (sa) ? (sa->len - sizeof(SockAddress)) : 0; 
	}
};

struct RawMessageSfltSetOption : public RawMessageBase	
{
	//	static errno_t	SetOption(void *cookie, socket_t so, sockopt_t opt);
	
	UInt64 so;
	inline void Init(UInt64 so)
	{
		RawMessageBase::Init(sizeof(RawMessageSfltSetOption), MessageTypeSfltSetOption);
		
		this->so = so;
	}
};

struct RawMessageSfltGetOption : public RawMessageBase	
{
	//	static errno_t	GetOption(void *cookie, socket_t so, sockopt_t opt);
	UInt64 so;
	
	inline void Init(UInt64 so)
	{
		RawMessageBase::Init(sizeof(RawMessageSfltGetOption), MessageTypeSfltGetOption);
		
		this->so = so;
	}
};

struct RawMessageSfltListen : public RawMessageBase		
{
	//	static errno_t	Listen(void *cookie, socket_t so);
	UInt64 so;
	
	inline void Init(UInt64 so)
	{
		RawMessageBase::Init(sizeof(RawMessageSfltListen), MessageTypeSfltListen);
		
		this->so = so;
	}
};

struct RawMessageSfltIoctl : public RawMessageBase		
{
	//	static errno_t	Ioctl(void *cookie, socket_t so, u_int32_t request, const char* argp);
	UInt64 so;
	UInt32 request;
	
	inline void Init(UInt64 so, UInt32 request)
	{
		RawMessageBase::Init(sizeof(RawMessageSfltIoctl), MessageTypeSfltIoctl);
		
		this->so = so;
		this->request = request;
	}
};

struct RawMessageSfltAccept : public RawMessageBase		
{
	//	static errno_t	Accept(void *cookie, socket_t so_listen, socket_t so, const sockaddr *local, const sockaddr *remote);
	UInt64 soListen;
	UInt64 so;
	
	UInt8 data[4];//local , remote
	
	inline void Init(UInt16 size, UInt64 soListen, UInt64 so, const SockAddress *local, const SockAddress *remote)
	{
		RawMessageBase::Init(size, MessageTypeSfltAccept);
		
		this->soListen = soListen;
		this->so = so;
		
		//TODO: copy sockaddr structs
	}
	
	inline SockAddress* GetLocal()
	{
		return (SockAddress*)data;
	}
	
	inline SockAddress* GetRemote()
	{
		return (SockAddress*)(data + GetLocal()->len);
	}
	
	inline static UInt16 GetNeededSize(SockAddress *local, SockAddress *remote)
	{
		return sizeof(RawMessageSfltAccept) + (local ? local->len : 0) + (remote ? remote->len : 0); //-4
	}
};

struct RawMessageClientAction : public RawMessageBase
{
	UInt32 messageId;
	
	inline void Init(UInt16 size, UInt16 type, UInt32 messageId)
	{
		RawMessageBase::Init(size, type);
		this->messageId = messageId;
	}
};

struct RawMessageClientActionResponce : public RawMessageBase
{
	UInt32 unitId;
	UInt32 clientMessageId;
	UInt32 actionState;
	
	inline void Init(UInt16 size, UInt16 type, UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{
		RawMessageBase::Init(size, type);
		this->unitId = unitId;
		this->clientMessageId = clientMessageId;
		this->actionState = actionState;
	}
};

struct RawMessageText : public RawMessageBase
{
	char textBuffer[4];
};

struct RawMessageAskRule : public RawMessageBase
{
	UInt16 processNameOffset;//0 for all
	UInt16 filePathOffset;//0 for all
	
	UInt16 sockDomain;//0 for all
	UInt16 sockType;//0 for all
	UInt16 sockProtocol;// 0 fro all	
	UInt16 fromSockAddressOffset;// 0 for all
	UInt16 toSockAddressOffset;
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	
	UInt32 pid;
	UInt32 uid;
	
	char buffer[4];
};

struct RawMessageRuleAdded : public RawMessageClientActionResponce
{
	UInt32 ruleId;//rule
	
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState , UInt32 ruleId)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRuleAdded), MessageTypeRuleAdded, unitId, clientMessageId, actionState); 
		this->ruleId = ruleId;
	}
};

struct RawMessageRuleDeleted : public RawMessageClientActionResponce
{
	UInt32 ruleId;//rule

	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState , UInt32 ruleId)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRuleDeleted), MessageTypeRuleDeleted, unitId, clientMessageId, actionState); 
		this->ruleId = ruleId;
	}
};

struct RawMessageRuleDeactivated : public RawMessageClientActionResponce
{
	UInt32 ruleId;//rule
	
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState , UInt32 ruleId)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRuleDeactivated), MessageTypeRuleDeactivated, unitId, clientMessageId, actionState); 
		this->ruleId = ruleId;
	}
};

struct RawMessageRuleActivated : public RawMessageClientActionResponce
{
	UInt32 ruleId;//rule

	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState , UInt32 ruleId)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRuleActivated), MessageTypeRuleActivated, unitId, clientMessageId, actionState); 
		this->ruleId = ruleId;
	}
};

struct RawMessageRegistredForAsk : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRegistredForAsk), MessageTypeRegistredForAsk, unitId, clientMessageId, actionState); 
	}
};

struct RawMessageUnregistredAsk : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageUnregistredAsk), MessageTypeUnregistredAsk, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageRegistredForInfoRule : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRegistredForInfoRule), MessageTypeRegistredForInfoRule, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageUnregistredInfoRule : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageUnregistredInfoRule), MessageTypeUnregisterInfoRule, unitId, clientMessageId, actionState); 
	}	
};


//////
struct RawMessageRegistredForInfoSocket : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageRegistredForInfoSocket), MessageTypeRegistredForInfoSocket, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageUnregistredInfoSocket : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageUnregistredInfoSocket), MessageTypeUnregisterInfoSocket, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageFirewallActivated : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageFirewallActivated), MessageTypeFirewallActivated, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageFirewallDeactivated : public RawMessageClientActionResponce
{
	inline void Init(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
	{ 
		RawMessageClientActionResponce::Init(sizeof(RawMessageFirewallDeactivated), MessageTypeFirewallDeactivated, unitId, clientMessageId, actionState); 
	}	
};

struct RawMessageFirewallClosing : public RawMessageBase 
{
	inline void Init(){ RawMessageBase::Init(sizeof(RawMessageFirewallClosing), MessageTypeFirewallClosing);};
};

/////
struct RawMessageSocketData : public RawMessageBase
{
	UInt8		stateOperation;
	//UInt8		fromAddressSize;
	//UInt8		toAddressSize;
	//UInt8		processNameSize;
	UInt8		direction;
	UInt32		stateByRuleId;
	UInt32		pid;//pid_t
	UInt32		uid;//uid_t
	UInt64		so;//socket_t
	UInt32		packets;
	UInt32		bytes;
	char		data[3];//from address, to address ?? process name
	
	inline void Init(UInt16 size, UInt8 direction, UInt8 stateOperation, UInt32 stateByRuleId, UInt32 pid, UInt32 uid,UInt64 so, UInt32 packets, UInt32 bytes, const SockAddress *from, const SockAddress *to, const char *processName, int processNameSize)
	{
		int currentOffset = 0;
		
		RawMessageBase::Init(size, MessageTypeSocketData);

		this->stateOperation = stateOperation;
		this->stateByRuleId = stateByRuleId;
		this->pid = pid;
		this->uid = uid;
		this->so = (UInt64)so;
		this->packets = packets;
		this->bytes = bytes;
		this->direction = direction;
		
		if(!from)
			from = &dummySockAddress;
		
		memcpy(this->data + currentOffset, from, from->len);
		currentOffset += from->len;

		if(!to)
			to = &dummySockAddress;
		
		memcpy(this->data + currentOffset, to, to->len);
		currentOffset += to->len;
		
//		if(processNameSize != 0)
//		{
//			this->processNameSize = processNameSize;
//			memcpy(this->data + currentOffset, processName, this->processNameSize);
//			currentOffset += this->processNameSize;
//		}
//		else
//		{
//			this->processNameSize = 0;
//			*(this->data + currentOffset) = 0;
//		}		
	}
	
	inline SockAddress* GetFromSocketAddress()
	{
		return (SockAddress*)this->data;
	}
	
	inline SockAddress* GetToSocketAddress()
	{
		return (SockAddress*)(this->data + GetFromSocketAddress()->len);
	}
	
	inline char* GetProcessName()
	{
		return 0;
	}
	
};

struct RawMessageSocketOpen : public RawMessageBase
{
	
};

struct RawMessageSocketClosed : public RawMessageBase
{
	
};


#pragma mark Client messages
struct RawMessageAddRule : public RawMessageClientAction 
{
	UInt32 id;
	UInt16 processNameOffset;//0 for all
	UInt16 filePathOffset;//0 for all
	
	UInt16 sockDomain;//0 for all
	UInt16 sockType;//0 for all
	UInt16 sockProtocol;// 0 fro all	
	UInt16 sockAddressOffset;// 0 for all
	
	UInt8 direction;//0 both. 1 incoming, 2 outgoung
	UInt8 allow;//0 denny, 1 allow
	UInt8 state;
	char buffer[1];
	
	char *GetProcessName(){ return (char*)this + processNameOffset; }
	char *GetFilePath(){ return (char*)this + filePathOffset;}
	SockAddress *GetSockAddress(){ return (SockAddress*) ((sockAddressOffset) ? (char*)this + sockAddressOffset : 0);}
	
	//TODO: by parent struct initialization
};

struct RawMessageDeleteRule : public RawMessageClientAction 
{
	UInt32 ruleId;
	
	inline void Init(UInt32 messageId, UInt32 ruleId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageDeleteRule), MessageTypeDeleteRule, messageId);
		this->ruleId = ruleId;
	}
};

struct RawMessageActivateRule : public RawMessageClientAction 
{
	UInt32 ruleId;
	
	inline void Init(UInt32 messageId, UInt32 ruleId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageActivateRule), MessageTypeActivateRule, messageId);
		this->ruleId = ruleId;
	}
};

struct RawMessageDeactivateRule : public RawMessageClientAction 
{
	UInt32 ruleId;
	
	inline void Init(UInt32 messageId, UInt32 ruleId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageDeactivateRule), MessageTypeDeactivateRule, messageId);
		this->ruleId = ruleId;
	}
};

struct RawMessageActivateFirewall : public RawMessageClientAction 
{
	inline void Init(UInt32 messageId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageActivateFirewall), MessageTypeActivateFirewall, messageId);
	}
};

struct RawMessageDeactivateFirewall : public RawMessageClientAction 
{
	inline void Init(UInt32 messageId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageDeactivateFirewall), MessageTypeDeactivateFirewall, messageId);
	}
};

struct RawMessageRegisterForAsk : public RawMessageClientAction 
{
	inline void Init(UInt32 messageId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageRegisterForAsk), MessageTypeRegisterForAsk, messageId);
	}
};

struct RawMessageUnregisterAsk : public RawMessageClientAction  
{
	inline void Init(UInt32 messageId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageUnregisterAsk), MessageTypeUnregisterAsk, messageId);
	}
};

struct RawMessageRegisterForInfoRule : public RawMessageClientAction  
{
	inline void Init(UInt32 messageId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageRegisterForInfoRule), MessageTypeRegisterForInfoRule, messageId);
	}
};

struct RawMessageUnregisterInfoRule : public RawMessageClientAction  
{
	inline void Init(UInt32 messageId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageUnregisterInfoRule), MessageTypeUnregisterInfoRule, messageId);
	}
};

struct RawMessageRegisterForInfoSocket : public RawMessageClientAction  
{
	inline void Init(UInt32 messageId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageRegisterForInfoSocket), MessageTypeRegisterForInfoSocket, messageId);
	}
};

struct RawMessageUnregisterInfoSocket : public RawMessageClientAction  
{
	inline void Init(UInt32 messageId)
	{
		RawMessageClientAction::Init(sizeof(RawMessageUnregisterInfoSocket), MessageTypeUnregisterInfoSocket, messageId);
	}
};

#endif
