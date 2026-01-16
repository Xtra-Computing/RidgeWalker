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
#include "kernel_vcache_mem.h"
#include "end_ctrl.h"


#define SIZE_OF_ROW_LIST    (16384)
#define SIZE_OF_MERGE       (1024 * 1024  * 64)






#define SEED  (1033)
int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);

    router_sink      krnl_router_sink(acc, 0);
    router_source    krnl_router_source(acc, 0);
    end_ctrl         krnl_end_ctrl[2];
    vcache_mem       krnl_vcache_mem[2];

    krnl_router_source.init(SIZE_OF_MERGE, 1024, 0 , (SIZE_OF_ROW_LIST * 2 - 1),  (SIZE_OF_ROW_LIST * 2 - 1), SEED , true);
    krnl_router_sink.init(0, true, SIZE_OF_MERGE);


    for (int j = 0; j < 2 ; j ++)
    {

        vertex_descriptor_vector_t row(SIZE_OF_ROW_LIST);
        for (auto i = 0; i < SIZE_OF_ROW_LIST; i++) {
            vertex_descriptor_t v;
            v.start = j + i;
            v.size  = 1;
            row[i] = v;
        }
        krnl_vcache_mem[j] =  vcache_mem(acc, j, 0);
        krnl_vcache_mem[j].init(row);
        krnl_end_ctrl[j] = end_ctrl(acc, j);

    }

    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();

        for (int j = 0; j < 2 ; j ++)
            krnl_vcache_mem[j].async_schedule_task();


        krnl_router_sink.async_schedule_task();
        krnl_router_source.async_schedule_task();
        krnl_router_source.async_join();
        krnl_router_sink.async_join();

        for (int j = 0; j < 2 ; j ++)
        {   krnl_end_ctrl[j].init(CTRL_END_OF_STREAM);
            krnl_end_ctrl[j].async_schedule_task();
        }
        for (int j = 0; j < 2 ; j ++)
        {
            krnl_end_ctrl[j].async_join();
        }

        for (int j = 0; j < 2 ; j ++)
            krnl_vcache_mem[j].async_join();

        stop = getCurrentTimestamp();
        printf("exec_time: %lf , thr %lf MStep/s\n", (stop - start), SIZE_OF_MERGE/(stop - start)/1000/1000);
    }
    return 0;
}


#if 0
int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);
    auto q = acc.alloc_task_queue();

    router_sink      krnl_router_sink[NUM_OF_SWITCH_STREAMS];
    router_source    krnl_router_source[NUM_OF_SWITCH_STREAMS];
    vcache_mem       krnl_vcache_mem[NUM_OF_SWITCH_STREAMS * 2];
    end_ctrl         krnl_end_ctrl(acc, 0);

    uint32_t         golden_size[NUM_OF_SWITCH_STREAMS];
    for (int j = 0; j < 2 ; j ++)
    {
        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++)
        {
            vertex_descriptor_vector_t row(SIZE_OF_ROW_LIST);
            for (auto i = 0; i < SIZE_OF_ROW_LIST; i++) {
                vertex_descriptor_t v;
                v.start = i * j;
                v.size  = i + 1;
                row[i] = v;
            }
            krnl_vcache_mem[j * NUM_OF_SWITCH_STREAMS + i] =  vcache_mem(acc, j, i);
            krnl_vcache_mem[j * NUM_OF_SWITCH_STREAMS + i].init(row);
        }
    }
    sleep(1);


    for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
    {
        krnl_router_sink[i] =   router_sink(acc, i);
        krnl_router_source[i] =  router_source(acc, i);
        krnl_router_source[i].init(SIZE_OF_MERGE, 1024, 0 , (SIZE_OF_ROW_LIST - 1),  (SIZE_OF_ROW_LIST - 1), 103 + i, true);
    }
    sleep(1);

    for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++)
    {
        golden_size[i] =  0;
        for (int j = 0; j < NUM_OF_SWITCH_STREAMS; j ++)
        {
            golden_size[i] += krnl_router_source[j].simulate_size(i, (NUM_OF_SWITCH_STREAMS - 1));
        }
        krnl_router_sink[i].init(0, true, golden_size[i]);
    }




    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();

        for (int j = 0; j < 2 ; j ++)
        {
            for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++)
            {
                krnl_vcache_mem[j * NUM_OF_SWITCH_STREAMS + i].async_schedule_task();
            }
        }
        sleep(1);

        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
        {
            krnl_router_sink[i].async_schedule_task();
        }
        sleep(1);
        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
        {
            krnl_router_source[i].async_schedule_task();
        }
        sleep(1);

        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
        {
            krnl_router_source[i].async_join();
        }

        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i++)
        {
            krnl_router_sink[i].async_join();
        }
        krnl_end_ctrl.init(CTRL_END_OF_STREAM);
        krnl_end_ctrl.async_schedule_task();
        krnl_end_ctrl.async_join();


        for (int j = 0; j < 2 ; j ++)
        {
            for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++)
            {
                krnl_vcache_mem[j * NUM_OF_SWITCH_STREAMS + i].async_join();
            }
        }


        stop = getCurrentTimestamp();
        printf("exec_time: %lf \n", (stop - start));
    }
}

#endif


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
