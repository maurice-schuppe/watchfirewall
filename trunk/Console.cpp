#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <getopt.h>
#include <sys/sysctl.h>
#include <sys/vnode.h>


//#include<iostream>

#include "Console.h"
#include "ServerConnection.h"

ServerConnection serverConnection;


/*
 SignalHandler - implemented to handle an interrupt from the command line using Ctrl-C.
 */
static void SignalHandler(int sigraised)
{
//#if DEBUG
    //printf("\ntcplog Interrupted - %d\n", sigraised); // note - printf is unsupported function call from a signal handler
//#endif
		
	serverConnection.Close();
    // exit(0) should not be called from a signal handler.  Use _exit(0) instead
    //
    _exit(0);
}


struct point
{
	double x;
	double y;
};

int
find_position(point *pointP, point *points, size_t size_points)
{
	int count = 0;
	for(int i = 0; i  < size_points; i++)
	{
		point *point1 = &points[i];
		point *point2 = &points[(i+1)%size_points];		
		
		if(point1->y == point2->y)
		{
			if((pointP->y == point1->y) && (abs(point1->x - pointP->x) + abs(point2->x - pointP->x) == abs(point2->x - point1->x)))
				return 0;
			
			continue;
		}
		
		double x = (point2->x - point1->x)*(pointP->y - point1->y) / (point2->y - point1->y) - point1->x;
		
		if(pointP->x == x)
			return 0;
		count += (pointP->x < x) ? 1 : 0;
	}
	
	return (count%2) ?  1 : -1;	
}

static char *
getprocname(pid_t pid)
{
	size_t len = sizeof(struct kinfo_proc);
	static int name[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, 0 };
	static struct kinfo_proc kp;
	name[3] = pid;
	kp.kp_proc.p_comm[0] = '\0';
	
	if (sysctl((int *)name, sizeof(name)/sizeof(*name), &kp, &len, NULL, 0))
		return NULL;
		
		if (kp.kp_proc.p_comm[0] == 0)
			return "exited?";

	return kp.kp_proc.p_comm;
}

void
test(pid_t pid)
{
	int i, mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, 0 };
	size_t len ;// = sizeof(struct kinfo_proc);;
	struct kinfo_proc kp;
	
	/* Fill out the first three components of the mib */ len = 4;
	//sysctlnametomib("kern.proc.pid", mib, &len);
	
	/* Fetch and print entries for pid’s < 100 */ 
	for (i = 0; i < 100; i++) 
	{
		mib[3] = i;
		len = sizeof(kp);
		if (sysctl(mib, 4, &kp, &len, NULL, 0) == -1)
			perror("sysctl");
		else 
			if (len > 0)
			{	
				printf("exutable name is : %s\n", kp.kp_proc.p_comm);
				//printkproc(&kp);	static struct kinfo_proc watch_proc;
			}
	}
//	kp.kp_proc.p_comm[0] = '\0';
//	kp.kp_proc.p_textvp = NULL;
//	kp.kp_proc.p_pid = 242;
//	int result = 0;
//	
//	static char testName[]="kern.proc.pid.      ";
//	
//	for(i = 0; i < 100; i++)
//	{	
//		sprintf(testName + 14, "%d", i);
//		if ((result = sysctlbyname(testName, &kp, &len, NULL, 0)) == 0)
//		{
//			static char name_buffer[1001];
//			//int len1 = 1000;
//			name_buffer[0] = '\0';
//			
//			printf("client exutable vnode : 0x%x", kp.kp_proc.p_textvp);
//			
//			//vn_getpath (watch_proc.kp_proc.p_textvp, name_buffer, &len1);// proc.kp_proc.p_textvp
//			
//			printf("client executable is : %s\n", name_buffer);
//			
//		}
//		else
//		{		
//			perror("sysctl");
//		}
//	}
}

char buffer[8*1024];

//test forum function


void printSum(unsigned number, unsigned from, unsigned count)
{
	printf("%d = %d", number, from);
	for(unsigned j = 1 ; j < count; j++)
		printf(" + %d", from + j);
	
	printf("\n");	
}

int findx(unsigned val, unsigned from)
{
	if(val < from)
		return 0;
	
	if(val == from)
		return from;
	
	return findx(val - from, from + 1);
}


//int findm(unsigned val, unsigned *from)
//{
//	if(val < from)
//		return 0;
//	
//	//TODO: refactor
//	if(val == from)
//		return from;
//	
//	++(*from);
//	return findx(val, from);
//}


int find(unsigned val, unsigned *count)
{	
	for(unsigned m = ((*count)*(*count -1))/2; val >= m ; m+=++(*count) - 1)
	{
		unsigned f = val -m;
		if(f%(*count) == 0)
			return f/(*count);
	}
	
	return 0;
}

int findp(unsigned val, unsigned *count)
{	
	unsigned m = ((*count)*(*count -1))/2;
		
	if(val < m)
		return 0;
		
	unsigned f = val -((*count)*(*count -1))/2;
	if(f%(*count) == 0)
		return f/(*count);
		
	++(*count);
	return findp(val, count);	
}


#include <stdio.h>

typedef struct _s
{
	struct _s *prev;
	unsigned val;
}s;


void ggg(unsigned val, s *d)
{
	if(!val)
	{
		for(;d; d= d->prev)
			printf("%u ", d->val);
		
		printf("\n");
		return;
	}	
	
	for(s m = {d, 1};m.val <= val && m.val <= (d ? d->val : unsigned(-1)); m.val++)	
		ggg(val - m.val, &m);
			
}



void gg(unsigned val, s *d)
{
	if(!val)
	{
		for(;d; d= d->prev)
			printf("%u ", d->val);
		
		printf("\n");
		return;
	}	
	
	s m = {d, (d && d->val < val) ? d->val : val}; 
	do	
		gg(val - m.val, &m);
			while(--m.val);		
}

void g(unsigned val)
{
	ggg(val, NULL);
}

//int main()
//{		
//	g(6);
//	return 0;
//}	

//end test forum function

int main()
{	
	//test forum function
	
	g(6);
	return 0;
	
	unsigned from ,o = 10;
	from = find(99, &o);
	
	for(unsigned val =2 ; val < 100; val++)
	{
		unsigned last;
		for(int from = 1; from < val; from++)
			if(last = findx(val, from))
				printSum(val, from, last - from + 1);		
	}	
	
	for(unsigned val =10000000 ; val < 10000100; val++)
	{
		unsigned from, count;
		for(count = 2 ;from = findp(val, &count); count++)
			printSum(val, from, count);
	}
	return 0;
	
	//end test forum function
	
	sig_t oldHandler = signal(SIGINT, SignalHandler);
    if (oldHandler == SIG_ERR)
        printf("Could not establish new signal handler");
	
	//test(242);
	
//	point points[] = {{-1,-1},{1,-1},{1,1},{-1,1}};
//	point p ={0,0};
//	point f ={1,0};
//	point d = {23,4};
//	point l = {-1,-1};
//	point s = {0, 0.5};
//	
//	int result = 22;
//	result = find_position(&p, points, 4);
//	result = find_position(&f, points, 4);
//	result = find_position(&d, points, 4);
//	result = find_position(&l, points, 4);
//	result = find_position(&s, points, 4);
	
	//test
	
	//char *name = NULL;
	//name = getprocname(226);
	
	//end test
	
	serverConnection.Open();
	
	//serverConnection.Send("test message", 13);
		
	
	//printf("test message %d", 23);
	
	
	//char *data = (char*)malloc(1001);
	int n;
	while ((n = recv(serverConnection.gSocket, buffer, 8 * 1024 , 0)) != 0)
	{
		if(n == -1)
		{
			printf("error recieve data");
			break;
		}
				
		if(!strcmp(buffer,"exit"))
			break;
		
		
		for(int k = 0; k < n; k+=106)
			printf(buffer + k);	
		
	}

	serverConnection.Close();
	printf("Connection Closed\n");
	//free(data);
	
	int a;
	scanf("%d",&a);

	
	return 0;
}