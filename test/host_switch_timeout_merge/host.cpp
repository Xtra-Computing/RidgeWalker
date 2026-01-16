#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>

#include "rn_test.h"


#include "kernel_router_sink.h"
#include "kernel_router_source.h"


#define SIZE_OF_MERGE (1024 * 16)

#ifndef NUM_OF_SWITCH_STREAMS
#error "No define of NUM_OF_SWITCH_STREAMS"
#endif


int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);
    auto q = acc.alloc_task_queue();

    router_sink      krnl_router_sink;
    router_source    krnl_router_source[NUM_OF_SWITCH_STREAMS];
    uint32_t         golden_size[NUM_OF_SWITCH_STREAMS];

    krnl_router_sink   =  router_sink(acc, 0);

    for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
    {

        krnl_router_source[i] =  router_source(acc, i);
        krnl_router_source[i].init(SIZE_OF_MERGE, 1024,128 + i , 1, i, 103 + i, true);
    }

    krnl_router_sink.init(0, true, SIZE_OF_MERGE);
    krnl_router_sink.set_size(0, 1024);


    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();

        krnl_router_sink.schedule_task(q);

        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
        {
            krnl_router_source[i].schedule_task(q);
        }

        q.finish();
        krnl_router_sink.get_res();

        stop = getCurrentTimestamp();
        printf("exec_time: %lf \n", (stop - start));
    }
}



#if 0

int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);
    auto q = acc.alloc_task_queue();

    rn_test   p_rn_test[NUM_PE];

    for (int i = 0; i < NUM_PE ; i++)
    {
        p_rn_test[i] = rn_test(acc, i + 2);
        p_rn_test[i].init();
    }
    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();

        for (int i = 0; i < NUM_PE ; i++)
        {
            p_rn_test[i].schedule_task(q);
        }

        q.finish();
        stop = getCurrentTimestamp();
        printf("exec_time: %lf  thr: %lf M request/s\n",
               (stop - start),
               NUM_PE * 1024 * 1024 * 64 / (stop - start) / 1000 / 1000 );
    }
}



#endif
