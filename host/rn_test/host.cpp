#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>

#include "rn_test.h"

#define  NUM_OF_RN (1024 * 1024)

#include "mem_access.h"
#include "mem_data_sink.h"
#include "rn_cmd_gen.h"

#ifndef NUM_PE
#error "need define  NUM_PE in CPP_FLAGS"
#endif


#ifndef MEM_TEST_INTERFACE_WIDTH
#error "need define  MEM_TEST_INTERFACE_WIDTH in both CPP_FLAGS and VPP_FLAGS"
#endif

#ifndef MEM_BURST_BUFFER_SIZE
#error "need define  MEM_BURST_BUFFER_SIZE in both CPP_FLAGS and VPP_FLAGS"
#endif


int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);
    auto q = acc.alloc_task_queue();

    mem_access     krnl_mem_access[NUM_PE];
    mem_data_sink  krnl_mem_data_sink[NUM_PE];
    rn_cmd_gen     krnl_rn_cmd_gen[NUM_PE];



    for (int i = 0; i < NUM_PE ; i++)
    {
        krnl_mem_access[i] = mem_access(acc, i);
        krnl_mem_access[i].init();

        krnl_mem_data_sink[i] = mem_data_sink(acc, i);
        krnl_mem_data_sink[i].init();

        krnl_rn_cmd_gen[i] = rn_cmd_gen(acc, i);
        krnl_rn_cmd_gen[i].init();
    }



    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;


        for (int i = 0; i < NUM_PE ; i++)
        {
            krnl_mem_access[i].schedule_task(q);
            krnl_mem_data_sink[i].schedule_task(q);
        }
        start = getCurrentTimestamp();

        for (int i = 0; i < NUM_PE ; i++)
        {
            krnl_rn_cmd_gen[i].schedule_task(q);
        }


        q.finish();
        stop = getCurrentTimestamp();
        DEBUG_PRINTF("exec_time: %lf  thr: %lf M request/s\n",
               (stop - start),
               NUM_PE * NUM_OF_RN / (stop - start) / 1000 / 1000 );
        DEBUG_PRINTF("bandwidth utlization: %lf GB/s\n",
               NUM_PE * NUM_OF_RN / (stop - start) / 1000 / 1000 *MEM_TEST_INTERFACE_WIDTH *  MEM_BURST_BUFFER_SIZE/8 /1000);
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
