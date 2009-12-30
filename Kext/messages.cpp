#include "messages.h"
#include "messageType.h"

Message*
Message::CreateText(const char* format,...)
{
    va_list argList;
	
    if (!format)
        return 0;

	Message *message = new(255 + sizeof(RawMessageBase)) Message;
	
	if(!message)
		return NULL;
	
    va_start(argList, format);
	
    message->raw.size = vsnprintf((char*)&message->raw + sizeof(RawMessageBase), 254, format, argList) + sizeof(RawMessageBase) + 1;

    va_end (argList);

	message->raw.type = MessageTypeText;
	message->references = 1;
	
    return message;
}

SInt32 nextTextMessage;//TODO: only for debug

Message*
Message::CreateTextFromCookie(const char* message, SocketCookie* cookie)
{
	UInt64 time;
	clock_get_uptime(&time);
	UInt32 mc = OSIncrementAtomic(&nextTextMessage);
	return CreateText("%d %lld %11s %20s path: %s  so: %9d  pid: %3d  uid: %3d  domain: %3d  type: %3d  protocol: %3d", 
					  mc,
					  time,
					  message, 
					  cookie->application->processName->getCStringNoCopy(),
					  cookie->application->filePath->getCStringNoCopy(),
					  cookie->socket, 
					  cookie->application->pid, 
					  cookie->application->uid,
					  cookie->sockDomain,
					  cookie->sockType,
					  cookie->sockProtocol);
}

Message*	
Message::CreateSfltUnregistered()//??????
{
	Message *message = new(sizeof(RawMessageSfltUnregistered)) Message;
	if(message)
	{
		RawMessageSfltUnregistered* rawMessage = (RawMessageSfltUnregistered*)&message->raw;
		rawMessage->Init();
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltAttach(socket_t so)
{
	Message *message = new(sizeof(RawMessageSfltAttach)) Message;
	if(message)
	{
		RawMessageSfltAttach* rawMessage = (RawMessageSfltAttach*)&message->raw;
		rawMessage->Init((UInt64)so);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltDetach(socket_t so)
{
	Message *message = new(sizeof(RawMessageSfltDetach)) Message;
	if(message)
	{
		RawMessageSfltDetach* rawMessage = (RawMessageSfltDetach*)&message->raw;
		rawMessage->Init((UInt64)so);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltNotify(socket_t so, UInt16 event)
{
	Message *message = new(sizeof(RawMessageSfltNotify)) Message;
	if(message)
	{
		RawMessageSfltNotify* rawMessage = (RawMessageSfltNotify*)&message->raw;
		rawMessage->Init((UInt64)so, event);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltGetPeerName(socket_t so, sockaddr *sa)
{
	size_t neededSize = RawMessageSfltGetPeerName::GetNeededSize((SockAddress*)sa);
	Message *message = new(neededSize) Message;
	if(message)
	{
		RawMessageSfltGetPeerName* rawMessage = (RawMessageSfltGetPeerName*)&message->raw;
		rawMessage->Init(neededSize, (UInt64)so, (SockAddress*)sa);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltGetSockName(socket_t so, sockaddr *sa)
{
	size_t neededSize = RawMessageSfltGetSockName::GetNeededSize((SockAddress*)sa);
	Message *message = new(neededSize) Message;
	if(message)
	{
		RawMessageSfltGetSockName* rawMessage = (RawMessageSfltGetSockName*)&message->raw;
		rawMessage->Init(neededSize, (UInt64)so, (SockAddress*)sa);
		message->references = 1;
	}
	return message;
}


Message*	
Message::CreateSfltDataIn(socket_t so, const sockaddr *from)
{
	size_t neededSize = RawMessageSfltDataIn::GetNeededSize((SockAddress*)from);
	Message *message = new(neededSize) Message;
	if(message)
	{
		RawMessageSfltDataIn* rawMessage = (RawMessageSfltDataIn*)&message->raw;
		rawMessage->Init(neededSize, (UInt64)so, (SockAddress*)from);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltDataOut(socket_t so, const sockaddr *to)
{
	size_t neededSize = RawMessageSfltDataOut::GetNeededSize((SockAddress*)to);
	Message *message = new(neededSize) Message;
	if(message)
	{
		RawMessageSfltDataOut* rawMessage = (RawMessageSfltDataOut*)&message->raw;
		rawMessage->Init(neededSize, (UInt64)so, (SockAddress*)to);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltConnectIn(socket_t so, const sockaddr *from)
{
	size_t neededSize = RawMessageSfltConnectIn::GetNeededSize((SockAddress*)from);
	Message *message = new(neededSize) Message;
	if(message)
	{
		RawMessageSfltConnectIn* rawMessage = (RawMessageSfltConnectIn*)&message->raw;
		rawMessage->Init(neededSize, (UInt64)so, (SockAddress*)from);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltConnectOut(socket_t so, const sockaddr *to)
{
	size_t neededSize = RawMessageSfltConnectOut::GetNeededSize((SockAddress*)to);
	Message *message = new(neededSize) Message;
	if(message)
	{
		RawMessageSfltConnectOut* rawMessage = (RawMessageSfltConnectOut*)&message->raw;
		rawMessage->Init(neededSize, (UInt64)so, (SockAddress*)to);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltBind(socket_t so, const sockaddr *to)
{
	size_t neededSize = RawMessageSfltBind::GetNeededSize((SockAddress*)to);
	Message *message = new(neededSize) Message;
	if(message)
	{
		RawMessageSfltBind* rawMessage = (RawMessageSfltBind*)&message->raw;
		rawMessage->Init(neededSize, (UInt64)so, (SockAddress*)to);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltSetOption(socket_t so, sockopt_t opt)
{
	Message *message = new(sizeof(RawMessageSfltSetOption)) Message;
	if(message)
	{
		RawMessageSfltSetOption* rawMessage = (RawMessageSfltSetOption*)&message->raw;
		rawMessage->Init((UInt64)so);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltGetOption(socket_t so, sockopt_t opt)
{
	Message *message = new(sizeof(RawMessageSfltGetOption)) Message;
	if(message)
	{
		RawMessageSfltGetOption* rawMessage = (RawMessageSfltGetOption*)&message->raw;
		rawMessage->Init((UInt64)so);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltListen(socket_t so)
{
	Message *message = new(sizeof(RawMessageSfltListen)) Message;
	if(message)
	{
		RawMessageSfltListen* rawMessage = (RawMessageSfltListen*)&message->raw;
		rawMessage->Init((UInt64)so);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltIoctl(socket_t so, UInt32 request, const char* argp)
{
	Message *message = new(sizeof(RawMessageSfltIoctl)) Message;
	if(message)
	{
		RawMessageSfltIoctl* rawMessage = (RawMessageSfltIoctl*)&message->raw;
		rawMessage->Init((UInt64)so, request);
		message->references = 1;
	}
	return message;
}

Message*	
Message::CreateSfltAccept(socket_t so_listen, socket_t so, const sockaddr *local, const sockaddr *remote)
{
	size_t neededSize = RawMessageSfltAccept::GetNeededSize((SockAddress*)local, (SockAddress*)remote);
	Message *message = new(neededSize) Message;
	if(message)
	{
		RawMessageSfltAccept* rawMessage = (RawMessageSfltAccept*)&message->raw;
		rawMessage->Init(neededSize, (UInt64)so_listen, (UInt64)so, (SockAddress*)local, (SockAddress*)remote);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateAskRule(char* processName, char* filePath, UInt16 sockDomain, UInt16 sockType, UInt16 sockProtocol,	sockaddr* sockAddress, UInt8 direction,
					   pid_t pid, uid_t uid)
{
	//TODO: implement
	return NULL;
}

Message*
Message::CreateRegistredForInfoRule(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new(sizeof(RawMessageRegistredForInfoRule)) Message;
	if(message)
	{
		RawMessageRegistredForInfoRule* rawMessage = (RawMessageRegistredForInfoRule*)&message->raw;
		rawMessage->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateUnregistredInfoRule(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	//TODO: size
	Message *message = new(sizeof(RawMessageUnregistredInfoRule)) Message;
	if(message)
	{
		((RawMessageUnregistredInfoRule*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateRegistredForInfoSocket(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new(sizeof(RawMessageRegistredForInfoSocket)) Message;
	if(message)
	{
		((RawMessageRegistredForInfoSocket*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateUnregistredInfoSocket(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageUnregistredInfoSocket)) Message;
	if(message)
	{
		((RawMessageUnregistredInfoSocket*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateRegistredForAsk(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageRegistredForAsk)) Message;
	if(message)
	{
		((RawMessageRegistredForAsk*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateUnregistredAsk(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageUnregistredAsk)) Message;
	if(message)
	{
		((RawMessageUnregistredAsk*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateFirewallActivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageFirewallActivated)) Message;
	if(message)
	{
		((RawMessageFirewallActivated*)&message->raw)->Init(unitId, clientMessageId, actionState); 
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateFirewallDeactivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState)
{
	Message *message = new (sizeof(RawMessageFirewallDeactivated)) Message;
	if(message)
	{
		((RawMessageFirewallActivated*)&message->raw)->Init(unitId, clientMessageId, actionState);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateFirewallClosing()
{
	Message *message = new Message();
	if(message)
	{
		RawMessageFirewallClosing* rawMessageFirewallClosing = (RawMessageFirewallClosing*)&message->raw; 
		rawMessageFirewallClosing->Init(); 
		message->references = 1;
	}
	return message;
}


Message*
Message::CreateRuleAdded(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	Message* message = new (sizeof(RawMessageRuleAdded)) Message;
	if(message)
	{
		((RawMessageRuleAdded*)&message->raw)->Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

Message* 
Message::CreateRuleDeleted(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	Message* message = new (sizeof(RawMessageRuleDeleted)) Message;
	if(message)
	{
		((RawMessageRuleDeleted*)&message->raw)->Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}


Message*
Message::CreateRuleDeactivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	Message* message = new (sizeof(RawMessageRuleDeactivated)) Message;
	if(message)
	{
		((RawMessageRuleDeactivated*)&message->raw)->Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

Message*
Message::CreateRuleActivated(UInt32 unitId, UInt32 clientMessageId, UInt32 actionState, UInt32 ruleId)
{
	Message* message = new (sizeof(RawMessageRuleActivated)) Message;
	if(message)
	{
		((RawMessageRuleActivated*)&message->raw)->Init(unitId, clientMessageId, actionState, ruleId);
		message->references = 1;
	}
	
	return message;
}

Message*
Message::CreateSocketData(UInt8 direction, UInt8 stateOperation, UInt32 stateByRuleId, pid_t pid, uid_t uid, socket_t so, UInt32 packets, UInt32 bytes, sockaddr *from, sockaddr *to, OSString *processName)
{
	//calculate size
	int neddedSize = sizeof(RawMessageSocketData);//- 3
	int processNameSize = 0;
	const char *processNameC = NULL;
	
	if(from != NULL)
		neddedSize += from->sa_len;
	
	if(to != NULL)
		neddedSize += to->sa_len;
	
	if(processName != NULL)
	{
		processNameSize = processName->getLength();
		processNameC = processName->getCStringNoCopy();
		neddedSize += processNameSize;
	}
	
	//TODO: is real
	Message* message = new(neddedSize) Message; 
	if(message)
	{
		((RawMessageSocketData*)&message->raw)->Init(neddedSize, direction, stateOperation, stateByRuleId, pid, uid, (UInt64)so, packets, bytes, (SockAddress*)from, (SockAddress*)to, processNameC, processNameSize);
		message->references = 1;
	}
	
	return message;
}

//TODO: additional info: from who
Message*
Message::CreateSocketOpen(socket_t so)
{
	Message *message = new(sizeof(RawMessageSocketOpen)) Message;
	if(message)
	{
		RawMessageSocketOpen* rawMessage = (RawMessageSocketOpen*)&message->raw;
		rawMessage->Init((UInt64)so);
		message->references = 1;
	}
	return message;
}

Message*
Message::CreateSocketClosed(socket_t so)
{
	Message *message = new(sizeof(RawMessageSocketClosed)) Message;
	if(message)
	{
		RawMessageSocketClosed* rawMessage = (RawMessageSocketClosed*)&message->raw;
		rawMessage->Init((UInt64)so);
		message->references = 1;
	}
	return message;
}

