#include "helper.h"
#ifdef __USER_NET_SOCKET_CONFIG__

#else

#define __USER_NET_SOCKET_CONFIG__  hacc

#endif

#define __USER_SOCKET_TABLE_FILE__ socket_table.h


#define  __SOCKET_TABLE_FILE__ STR(config/__USER_NET_CONFIG__/__USER_SOCKET_TABLE_FILE__)
#include __SOCKET_TABLE_FILE__