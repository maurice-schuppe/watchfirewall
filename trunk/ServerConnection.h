#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/kern_control.h>

#define MYBUNDLEID		"com.janbird.watch.kext"

class ServerConnection 
{
	public :
		bool Open();
		bool Close();
		
		int	Send( const void* data, size_t size);
	
	ServerConnection()
	{
		gSocket = -1;
	}
	
	protected:
	private:
	
		sockaddr_ctl sc;
		//tl_stats tl_stats;
		socklen_t	size;
		ctl_info		m_ctl_info;
public:
	int			gSocket;// = -1;
	
};

#endif /* SERVER_CONNECTION_H */
