#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>

#include "rn_test.h"

#include "mem_access.h"
#include "mem_data_sink.h"
#include "mem_access_sim.h"

#ifdef MEM_CTRL_INTERFACE
#include "end_ctrl.h"
#endif


int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);
    auto q = acc.alloc_task_queue();

    mem_access          krnl_mem_access(acc, 0);
    mem_data_sink       krnl_mem_data_sink(acc, 0);
    mem_access_sim      krnl_mem_access_sim(acc, 0);

#ifdef MEM_CTRL_INTERFACE
    end_ctrl            krnl_end_ctrl(acc, 0);
#endif

    krnl_mem_access.init();
    krnl_mem_access_sim.init(1024*64,40*4,44*4,0x1, 0x1);
    krnl_mem_data_sink.init();
    krnl_end_ctrl.init(CTRL_END_OF_STREAM);


    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();

        krnl_mem_access.async_schedule_task();
        krnl_mem_data_sink.async_schedule_task();
        krnl_mem_access_sim.async_schedule_task();

        krnl_mem_access_sim.async_join();
        krnl_mem_data_sink.async_join();
        sleep(30);
        krnl_end_ctrl.async_schedule_task();
        krnl_end_ctrl.async_join();
        krnl_mem_access.async_join();


        stop = getCurrentTimestamp();
        printf("exec_time: %lf  thr: %lf M request/s\n",
               (stop - start),
               (krnl_mem_access_sim.get_num_of_access()) / (stop - start) / 1000 / 1000 );
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
