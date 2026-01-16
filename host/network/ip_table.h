
#include "helper.h"



#ifdef __USER_NET_CONFIG__

#else

#define __USER_NET_CONFIG__  hacc

#endif

#define __USER_IP_TABLE_FILE__ ip_table.h


#define  __IP_TABLE_FILE__ STR(config/__USER_NET_CONFIG__/__USER_IP_TABLE_FILE__)
#include __IP_TABLE_FILE__