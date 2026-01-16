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
#include "kernel_x4_router_sink.h"
#include "kernel_router_source.h"
#include "netconfig.hpp"


#define SIZE_OF_MERGE (1024 * 16 * 1024)



#ifndef ACC_START_ID
#define ACC_START_ID   (1)
#endif


int main(int argc, char *argv[]) {

    config_network(argc, argv, true);
    //exit(0);
    cl_accelerator acc[2];
    cl::CommandQueue q[2];
    router_sink      krnl_router_sink[2][NUM_OF_SWITCH_STREAMS];
    router_source    krnl_router_source[2][NUM_OF_SWITCH_STREAMS];
    for (int acc_id = 0; acc_id < 2; acc_id++)
    {
        acc[acc_id] =  cl_accelerator(argc, argv, acc_id + ACC_START_ID);
        q[acc_id] = acc[acc_id].alloc_task_queue();

        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
        {
            krnl_router_sink[acc_id][i]   =  router_sink(acc[acc_id], i);
            krnl_router_source[acc_id][i] =  router_source(acc[acc_id], i);
            krnl_router_source[acc_id][i].init(SIZE_OF_MERGE, 1024,  8  , 0, (NUM_OF_SWITCH_STREAMS * 2 - 1) , 103 + i, true);
        }

        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++)
        {
            krnl_router_sink[acc_id][i].init(0, true, 1024);
            krnl_router_sink[acc_id][i].set_size(0, 1024 * 1024 * 1024);
        }

    }



    //kernel_x4_router_sink.init(0, true, 1024);
    //kernel_x4_router_sink.set_size(0, 1024);


#define ACC_NUM (2)
    for (int iteration = 0; iteration < 1; iteration ++)
    {
        double start, stop;


        for (int acc_id = 0; acc_id < ACC_NUM; acc_id++)
        {
            for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
            {
                krnl_router_sink[acc_id][i].async_schedule_task(q[acc_id]);
            }
        }
        sleep(2);

        start = getCurrentTimestamp();
        for (int acc_id = 0; acc_id < ACC_NUM; acc_id++)
        {
            for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
            {
                krnl_router_source[acc_id][i].async_schedule_task(q[acc_id]);
            }
        }

        //kernel_x4_router_sink.schedule_task(q);
        for (int acc_id = 0; acc_id < ACC_NUM; acc_id++)
        {
            for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
            {
                krnl_router_source[acc_id][i].async_join();
            }
            //q[acc_id].finish();
        }
        stop = getCurrentTimestamp();



        for (int acc_id = 0; acc_id < ACC_NUM; acc_id++)
        {
            for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++) {
                krnl_router_sink[acc_id][i].async_join();
                krnl_router_sink[acc_id][i].get_res(1);
            }
        }

        //kernel_x4_router_sink.get_res(1);

        //stop = getCurrentTimestamp();
        printf("exec_time: %lf thr %lf \n", (stop - start), SIZE_OF_MERGE / (stop - start) / 1000 / 1000);
    }
    dump_package();
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
