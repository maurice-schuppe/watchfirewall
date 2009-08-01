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
			MessageBase *message = (MessageBase*)(buffer + k);
			
			switch (message->type) {
				case MessageTypeText:
					{
						MessageText* messageText = (MessageText*)message;
						printf("%s \n", messageText->textBuffer);
					}
					break;

				case MessageTypeAskRule:
					{
						MessageAskRule* messageAskRule = (MessageAskRule*)message;
					}
					break;
					
				case MessageTypeRuleAdded:
					{
						MessageRuleAdded* messageRuleAdded = (MessageRuleAdded*)message;
					}
					break;
					
				case MessageTypeRuleDeleted:
					{
						MessageRuleDeleted* messageRuleDeleted = (MessageRuleDeleted*)message;
					}
					break;
					
				case MessageTypeRuleDeactivated:
					{
						MessageRuleDeactivated* messageRuleDeactivated = (MessageRuleDeactivated*)message;
					}
					break;
					
				case MessageTypeRuleActivated:
					{
						MessageRuleActivated* messageRuleActivated = (MessageRuleActivated*)message;
					}
					break;
					
					
				case MessageTypeSocketDataIN:
					{
						MessageSocketDataIN* messageSocketDataIN = (MessageSocketDataIN*)message;
					}
					break;
					
				case MessageTypeSocketDataOUT:
					{
						MessageSocketDataOUT* messageSocketDataOUT = (MessageSocketDataOUT*)message;
					}
					break;
					
				case MessageTypeSocketOpen:
					{
						MessageSocketOpen* messageSocketOpen = (MessageSocketOpen*)message;
					}
					break;
					
				case MessageTypeSocketClosed:
					{
						MessageSocketClosed* messageSocketClosed = (MessageSocketClosed*)message;
					}
					break;
					
				case MessageTypeFirewallActivated:
					{
						MessageFirewallActivated* messageFirewallActivated = (MessageFirewallActivated*)message;
					}
					break;
					
				case MessageTypeFirewallDeactivated:
					{
						MessageFirewallDeactivated* messageFirewallDeactivated = (MessageFirewallDeactivated*)message;
					}
					break;
					
					
				case MessageTypeRegistredForAsk:
					{
						MessageRegistredForAsk* messageRegistredForAsk = (MessageRegistredForAsk*)message;
					}
					break;
					
				case MessageTypeUnregistredAsk:
					{
						MessageUnregistredAsk* messageUnregistredAsk = (MessageUnregistredAsk*)message;
					}
					break;
					
				case MessageTypeRegistredForInfoRule:
					{
						MessageRegistredForInfoRule* messageRegistredForInfoRule = (MessageRegistredForInfoRule*)message;
					}
					break;
					
				case MessageTypeUnregistredInfoRule:
					{
						MessageUnregistredInfoRule* messageUnregistredInfoRule = (MessageUnregistredInfoRule*)message;
					}
					break;
					
				case MessageTypeRegistredForInfoSocket:
					{
						MessageRegistredForInfoSocket* messageRegistredForInfoSocket = (MessageRegistredForInfoSocket*)message;
						printf("client registred for info rule: %s", messageRegistredForInfoSocket->actionState ? "success" : "error");
					}
					break;
					
				case MessageTypeUnregistredInfoSocket:
					{
						MessageUnregistredInfoSocket* messageUnregistredInfoSocket = (MessageUnregistredInfoSocket*)message;
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