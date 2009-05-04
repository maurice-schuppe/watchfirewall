#ifndef WATCH_MESSAGES_H
#define WATCH_MESSAGES_H


//from server
#define MESSAGE_TEXT 1 //dymmu
#define MESSAGE_ASK_RULE 2

#define MESSAGE_INFO_RULE_ADDED 3
#define MESSAGE_INFO_RULE_DELETED 4
#define MESSAGE_INFO_RULE_DISABLED 5
#define MESSAGE_INFO_RULE_ENABLED 6

#define MESSAGE_INFO_SOCKET_REJECTED 7
#define MESSAGE_INFO_SOCKET_ALLOWED 8
#define MESSAGE_INFO_SOCKET_DATA_IN 9
#define MESSAGE_INFO_SOCKET_DATA_OUT 10
#define MESSAGE_INFO_SOCKET_UP 11
#define MESSAGE_INFO_SOCKET_DOWN 12
#define MESSAGE_INFO_SOCKET_ASK_RULE 13

#define MESSAGE_INFO_FIREWALL_UP 14
#define MESSAGE_INFO_FIREWALL_DOWN 15

//from client
#define MESSAGE_DELETE_RULE 16
#define MESSAGE_ADD_RULE 17
#define MESSAGE_OPEN_FIREWALL 18
#define MESSAGE_CLOSE_FIREWALL 19
#define MESSAGE_REGISTER_FOR_ASK_RULE 20
#define MESSAGE_UNREGISTER_FROM_ASK_RULE 21
#define MESSAGE_REGISTER_FOR_INFO 22
#define MESSAGE_UNREGSTER_FROM_INFO 23

typedef struct _watch_message_header 
{
	u_int16_t length;
	u_int16_t message_type;	 
} watch_message_header;


//void create_string_message(char* text)
//{
//	int size = strlen(text);
//	char *buffer = OSMalloc(size + 4, osmalloc_tag);
//	((watch_message_header)buffer)->length = size + 4;
//	((watch_message_header)buffer)->message_type = 1;
//	strcpy((), )
//	
//}

#endif
