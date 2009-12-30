#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <getopt.h>
#include <sys/sysctl.h>
#include <sys/vnode.h>
#include <libkern/OSTypes.h>
#include "ServerConnection.h"

typedef void* socket_t;

#include "messageType.h"
#include "Console.h"


ServerConnection serverConnection;
char buffer[8*1024];

int SockAddressToString(const sockaddr* sa, char *buffer, int bufferLen)
{
//	const char *inet_ntop(int af, const void *restrict src,
//						  char *restrict dst, socklen_t size);
//	int inet_pton(int af, const char *restrict src, void *restrict dst);
	if(!sa)
		return 0;
	
	switch(sa->sa_family)
	{
		case AF_UNSPEC:
		{
		}
	}
	
	printf("");
	return 0;
}

/*
 SignalHandler - implemented to handle an interrupt from the command line using Ctrl-C.
 */
static void SignalHandler(int sigraised)
{
	serverConnection.Close();
    _exit(0);
}

int main()
{	
	sig_t oldHandler = signal(SIGINT, SignalHandler);
    if (oldHandler == SIG_ERR)
        printf("Could not establish new signal handler");

	serverConnection.Open();

	int n;
	while ((n = recv(serverConnection.gSocket, buffer, 8 * 1024 , 0)) != 0)
	{
		if(n == -1)
		{
			printf("error recieve data");
			break;
		}
			
		for(int k = 0; k < n;  )
		{
			RawMessageBase *message = (RawMessageBase*)(buffer + k);
			
			switch (message->type) {
				case MessageTypeText:
					{
						RawMessageText* messageText = (RawMessageText*)message;
						printf("%s \n", messageText->textBuffer);
					}
					break;

				case MessageTypeRequestRule:
					{
						RawMessageRequestRule* messageRequestRule = (RawMessageRequestRule*)message;
					}
					break;
					
				case MessageTypeRuleAdded:
					{
						RawMessageRuleAdded* messageRuleAdded = (RawMessageRuleAdded*)message;
					}
					break;
					
				case MessageTypeRuleDeleted:
					{
						RawMessageRuleDeleted* messageRuleDeleted = (RawMessageRuleDeleted*)message;
					}
					break;
					
				case MessageTypeRuleDeactivated:
					{
						RawMessageRuleDeactivated* messageRuleDeactivated = (RawMessageRuleDeactivated*)message;
					}
					break;
					
				case MessageTypeRuleActivated:
					{
						RawMessageRuleActivated* messageRuleActivated = (RawMessageRuleActivated*)message;
					}
					break;
					
				case MessageTypeSocketData:
					{
						RawMessageSocketData* messageSocketData = (RawMessageSocketData*)message;
					}
					break;
					
				case MessageTypeSocketOpen:
					{
						RawMessageSocketOpen* messageSocketOpen = (RawMessageSocketOpen*)message;
					}
					break;
					
				case MessageTypeSocketClosed:
					{
						RawMessageSocketClosed* messageSocketClosed = (RawMessageSocketClosed*)message;
					}
					break;
					
				case MessageTypeFirewallActivated:
					{
						RawMessageFirewallActivated* messageFirewallActivated = (RawMessageFirewallActivated*)message;
					}
					break;
					
				case MessageTypeFirewallDeactivated:
					{
						RawMessageFirewallDeactivated* messageFirewallDeactivated = (RawMessageFirewallDeactivated*)message;
					}
					break;
					
					
				case MessageTypeClientSubscribedAsaProviderOfRules:
					{
						RawMessageClientSubscribedAsaProviderOfRules* messageRegistredForAsk = (RawMessageClientSubscribedAsaProviderOfRules*)message;
					}
					break;
					
				case MessageTypeClientUnsubscribedAsaProviderOfRules:
					{
						RawMessageClientUnsubscribedAsaProviderOfRules* messageUnregistredAsk = (RawMessageClientUnsubscribedAsaProviderOfRules*)message;
					}
					break;
					
				case MessageTypeClientSubscribedToInfoRules:
					{
						RawMessageClientSubscribedToInfoRules* messageRegistredForInfoRule = (RawMessageClientSubscribedToInfoRules*)message;
					}
					break;
					
				case MessageTypeClientUnsubscribedFromInfoRules:
					{
						RawMessageClientUnsubscribedFromInfoRules* messageUnregistredInfoRule = (RawMessageClientUnsubscribedFromInfoRules*)message;
					}
					break;
					
				case MessageTypeClientSubscribedToInfoSockets:
					{
						RawMessageClientSubscribedToInfoSockets* messageRegistredForInfoSocket = (RawMessageClientSubscribedToInfoSockets*)message;
						printf("client registred for info rule: %s", messageRegistredForInfoSocket->actionState ? "success" : "error");
					}
					break;
					
				case MessageTypeClientUnsubscribedFromInfoSockets:
					{
						RawMessageClientUnsubscribedFromInfoSockets* messageUnregistredInfoSocket = (RawMessageClientUnsubscribedFromInfoSockets*)message;
					}
					break;
					
					
				case MessageTypeFirewallClosing:
					goto close;
				default:
					break;
			}
			k += message->size;
		}
	}

close:;
	serverConnection.Close();
	printf("Connection Closed\n");
	
	int a;
	scanf("%d",&a);

	
	return 0;
}