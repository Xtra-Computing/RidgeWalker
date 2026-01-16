#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct {
    socklen_t addr_size;
    int sockfd;
    struct sockaddr_in si_me;
    struct sockaddr_in server_addr;

    char* ip_address;
    int port;
    int last_value;

} net_barrier_t;

typedef struct
{
    const char      *ip;
    int             device_id;
    net_barrier_t   instance;
} net_barrier_lut_t;

#include "netconfig.hpp"
#include "barrier_table.h"




static net_barrier_t local_barrier;





void net_barrier_client_init(int device_id) {
    net_barrier_t * p_barrier = &local_barrier;
    if (device_id > 16)
    {
        printf("supported device_id less than 16\n");
        exit(1);
    }
    p_barrier->port = 5001 + device_id;
    p_barrier->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&p_barrier->si_me, 0, sizeof(p_barrier->si_me));
    p_barrier->si_me.sin_family = AF_INET;
    p_barrier->si_me.sin_port = htons(p_barrier->port);
    p_barrier->si_me.sin_addr.s_addr = INADDR_ANY;

    bind(p_barrier->sockfd, (struct sockaddr*)&p_barrier->si_me, sizeof(p_barrier->si_me));
}

void net_barrier_wait(void)
{
    net_barrier_t * p_barrier = &local_barrier;
    int buffer[256];
    memset(buffer, 0, sizeof(int) * 256);

    struct sockaddr_in  si_other;
    socklen_t addr_size;
    addr_size = sizeof(si_other);
    recvfrom(p_barrier->sockfd, (char *)buffer, 4, 0, (struct sockaddr*)& si_other, &addr_size);
    printf("[NET] triggered with %d\n", buffer[0]);
    return;
}


void net_barrier_trigger_init(void) {
    net_barrier_t * p_barrier = &local_barrier;

    for (uint32_t  i = 0; i < ARRAY_SIZE(net_barrier_lut); i++) {
        net_barrier_t * p_barrier = &net_barrier_lut[i].instance;
        net_barrier_lut_t * p_lut = &net_barrier_lut[i];

        p_barrier->sockfd = socket(PF_INET, SOCK_DGRAM, 0);
        memset(&p_barrier->server_addr, 0, sizeof(p_barrier->server_addr));
        p_barrier->server_addr.sin_family = AF_INET;
        p_barrier->server_addr.sin_port = htons(5001 + p_lut->device_id);
        p_barrier->server_addr.sin_addr.s_addr = inet_addr(p_lut->ip);

    }
}


void net_barrier_trigger(void)
{
    static int counter = 0;
    int buffer[256];
    memset(buffer, 0, sizeof(int) * 256);
    buffer[0] = counter;
    counter ++;

    for (uint32_t  i = 0; i < ARRAY_SIZE(net_barrier_lut); i++) {
        net_barrier_t * p_barrier = &net_barrier_lut[i].instance;
        net_barrier_lut_t * p_lut = &net_barrier_lut[i];
        sendto(p_barrier->sockfd, buffer, 4, 0, (struct sockaddr*)&p_barrier->server_addr,
               sizeof(p_barrier->server_addr));
        printf("[NET] triggering %s %d with %d\n",p_lut->ip, 5001 + p_lut->device_id, buffer[0]);
    }

}