#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <getopt.h>
#include <sys/sysctl.h>
#include <sys/vnode.h>
#include <libkern/OSTypes.h>

#include "messageType.h"
#include "Console.h"
#include "ServerConnection.h"

ServerConnection serverConnection;
char buffer[8*1024];

struct Message 
{
	UInt16 length;
	UInt16 type;
	
	char *buffer() { return ((char*)this) + 4;}
};

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

	int n, k;
	while ((n = recv(serverConnection.gSocket, buffer, 8 * 1024 , 0)) != 0)
	{
		if(n == -1)
		{
			printf("error recieve data");
			break;
		}
			
		for(k = 0; k < n;  )
		{
			Message *message = (Message*)(buffer + k);
			
			switch (message->type) {
				case MESSAGE_TEXT:
				{
					char *buf = message->buffer();
					printf("%s \n", buf);
					//fflush(stdout);
				}
					break;
				case MESSAGE_INFO_CLOSED:
					goto close;
				default:
					break;
			}
			k += message->length;
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