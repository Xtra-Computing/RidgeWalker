
#include "helper.h"



#ifdef __USER_NET_CONFIG__

#else

#define __USER_NET_CONFIG__  hacc

#endif

#define __USER_BARRIER_TABLE_FILE__ barrier_table.h


#define  __BARRIER_TABLE_FILE__ STR(config/__USER_NET_CONFIG__/__USER_BARRIER_TABLE_FILE__)
#include __BARRIER_TABLE_FILE__