#ifndef __NETBARRIER_HPP__
#define __NETBARRIER_HPP__



void net_barrier_client_init(int device_id);
void net_barrier_wait(void);

void net_barrier_trigger_init(void);
void net_barrier_trigger(void);


#endif /* __NETBARRIER_HPP__ */
