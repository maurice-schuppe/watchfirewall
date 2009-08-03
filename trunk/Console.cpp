#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <getopt.h>
#include <sys/sysctl.h>
#include <sys/vnode.h>
#include <libkern/OSTypes.h>
#include "ServerConnection.h"
#include "messageType.h"
#include "Console.h"


ServerConnection serverConnection;
char buffer[8*1024];

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

				case MessageTypeAskRule:
					{
						RawMessageAskRule* messageAskRule = (RawMessageAskRule*)message;
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
					
					
				case MessageTypeSocketDataIN:
					{
						RawMessageSocketDataIN* messageSocketDataIN = (RawMessageSocketDataIN*)message;
					}
					break;
					
				case MessageTypeSocketDataOUT:
					{
						RawMessageSocketDataOUT* messageSocketDataOUT = (RawMessageSocketDataOUT*)message;
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
					
					
				case MessageTypeRegistredForAsk:
					{
						RawMessageRegistredForAsk* messageRegistredForAsk = (RawMessageRegistredForAsk*)message;
					}
					break;
					
				case MessageTypeUnregistredAsk:
					{
						RawMessageUnregistredAsk* messageUnregistredAsk = (RawMessageUnregistredAsk*)message;
					}
					break;
					
				case MessageTypeRegistredForInfoRule:
					{
						RawMessageRegistredForInfoRule* messageRegistredForInfoRule = (RawMessageRegistredForInfoRule*)message;
					}
					break;
					
				case MessageTypeUnregistredInfoRule:
					{
						RawMessageUnregistredInfoRule* messageUnregistredInfoRule = (RawMessageUnregistredInfoRule*)message;
					}
					break;
					
				case MessageTypeRegistredForInfoSocket:
					{
						RawMessageRegistredForInfoSocket* messageRegistredForInfoSocket = (RawMessageRegistredForInfoSocket*)message;
						printf("client registred for info rule: %s", messageRegistredForInfoSocket->actionState ? "success" : "error");
					}
					break;
					
				case MessageTypeUnregistredInfoSocket:
					{
						RawMessageUnregistredInfoSocket* messageUnregistredInfoSocket = (RawMessageUnregistredInfoSocket*)message;
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
	//free(data);
	
	int a;
	scanf("%d",&a);

	
	return 0;
}