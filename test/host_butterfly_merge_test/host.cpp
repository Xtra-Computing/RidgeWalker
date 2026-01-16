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


#define SIZE_OF_MERGE (1024)


int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);
    auto q = acc.alloc_task_queue();

    router_sink     krnl_router_sink(acc, 0);
    router_source   krnl_router_source_1(acc, 0);
    router_source   krnl_router_source_2(acc, 1);


    krnl_router_sink.init(2, false, SIZE_OF_MERGE * 2);
    krnl_router_source_1.init(SIZE_OF_MERGE, 48, 40, 1, 0x1, rand(),false);
    krnl_router_source_2.init(SIZE_OF_MERGE, 40, 44, 2, 0x1, rand(),false);


    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();

        krnl_router_sink.schedule_task(q);
        krnl_router_source_1.schedule_task(q);
        krnl_router_source_2.schedule_task(q);

        q.finish();

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
