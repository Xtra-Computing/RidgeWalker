#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>

#include "rn_test.h"

#include "kernel_net_sink.h"
#include "kernel_router_single_sink.h"
#include "kernel_router_single_source.h"


#define SIZE_OF_MERGE (1024 * 16)


#define TEST_N_SOURCE (8)

int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);
    auto q = acc.alloc_task_queue();

    net_sink            krnl_router_sink_1;
    router_source       krnl_router_source[TEST_N_SOURCE];

    krnl_router_sink_1   =  net_sink(acc, 0);

    for (int i = 0; i < TEST_N_SOURCE; i ++) {
        krnl_router_source[i] =  router_source(acc, i);
    }


    std::vector<uint128_t, aligned_allocator<uint128_t> > dummy_rpa;

    int size_of_test = SIZE_OF_MERGE;

    for (int i = 0; i < size_of_test; i++)
    {
        uint128_t item = {0};
        item.data[0] = i;
        item.data[1] = 1;
        dummy_rpa.push_back(item);
    }

    for (int i = 0; i < TEST_N_SOURCE; i ++) {
        krnl_router_source[i].init(dummy_rpa);
    }

    krnl_router_sink_1.init(size_of_test * TEST_N_SOURCE / 4);

    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();

        krnl_router_sink_1.schedule_task(q);
        //krnl_router_sink_2.schedule_task(q);


        for (int i = 0; i < TEST_N_SOURCE; i ++) {
            krnl_router_source[i].schedule_task(q);
        }

        q.finish();
        krnl_router_sink_1.get_res();
        //krnl_router_sink_2.get_res();

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
