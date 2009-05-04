#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <sys/types.h>

#define WATCH_PROTOCOL_COMMAND_IN 1


typedef struct _watch_protocol_packet 
{	
	u_int16_t packet_number;
	u_int16_t packet_length;
	u_int16_t command;	
} watch_protocol_packet;

#endif /* PROTOCOL_H */