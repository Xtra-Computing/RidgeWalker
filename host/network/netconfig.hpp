#ifndef __NETCONFIG_HPP__
#define __NETCONFIG_HPP__

#include "helper.h"

#ifdef __USER_NET_CONFIG__

#define  NET_CONFIG_NAME  STR(__USER_NET_CONFIG__)

#else

#define  NET_CONFIG_NAME  "hacc"

#endif

int config_network(int argc, char *argv[], bool reconfig);
void dump_package(void);
void dump_full_dist(bool full_log);

#endif /* __NETCONFIG_HPP__ */
