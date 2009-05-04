
#include "ServerConnection.h"


#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/kern_control.h>
#include <sys/sys_domain.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


bool ServerConnection::Open()
{
	gSocket = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
	if (gSocket < 0)
		return false;
	
	bzero(&m_ctl_info, sizeof(struct ctl_info));
	strcpy(m_ctl_info.ctl_name, MYBUNDLEID);
	
	if (ioctl(gSocket, CTLIOCGINFO, &m_ctl_info) == -1)
		return false;
//	{
//		perror("ioctl CTLIOCGINFO");
//		exit(0);
//	} else
//		printf("ctl_id: 0x%x for ctl_name: %s\n", m_ctl_info.ctl_id, m_ctl_info.ctl_name);
	
	bzero(&sc, sizeof(struct sockaddr_ctl));
	sc.sc_len = sizeof(struct sockaddr_ctl);
	sc.sc_family = AF_SYSTEM;
	sc.ss_sysaddr = SYSPROTO_CONTROL;
	sc.sc_id = m_ctl_info.ctl_id;
	sc.sc_unit = 0;
	
	if (connect(gSocket, (struct sockaddr *)&sc, sizeof(struct sockaddr_ctl)))
		return false;
	
//	{
//		perror("connect");
//		exit(0);
//	}
	
	return true;
}

bool ServerConnection::Close()
{
	if(gSocket > 0)
		close (gSocket);
	gSocket = -1;
	return true;
}

int ServerConnection::Send( const void* data, size_t size)
{	
	return send(this->gSocket, data, size, 0);	
}
