#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>

#include "graph.h"
#include "util/command_parser.h"
#include "log.h"


#include "switch_data_struct.h"
#include "kernel_query_statistic.h"
#include "kernel_config_loader.h"
#include "kernel_rng_config_loader.h"


#include "netconfig.hpp"



#define SIZE_OF_ROW_LIST    (16384)

#ifdef __EMULATION__
#define SIZE_OF_MERGE       (1024 * 64)
#else
#define SIZE_OF_MERGE       (1024 * 1024 * 16* 2)
#endif

#define NUM_OF_CHN  (8)

#define QUERY_LEN   (80 - 1)


#ifndef NUM_OF_SWITCH_STREAMS
#error "No define of NUM_OF_SWITCH_STREAMS"
#endif



int prepare_graph(Graph &graph,    int partition_number,
                  std::vector< lvertex_desp_vector_t > &rp,
                  std::vector< lvertex_host_vector_t > &cl )
{
    rp.resize(0);
    cl.resize(0);
    for  (int i = 0; i < partition_number; i ++)
    {
        lvertex_desp_vector_t  rp_partition;
        rp.push_back(rp_partition);

        lvertex_host_vector_t cl_partition;
        cl.push_back(cl_partition);
    }
    pcg32_srandom(42u, 54u);

    for (auto i = 0; i < graph.num_vertices(); i++) {

        // shuffle, shift 1 for debug
        uint32_t target_cl_ch =  (pcg32_random()) & (partition_number - 1);

        // shuffle
        uint32_t target_rp_ch =  i & (partition_number - 1);

        lvertex_desp_host_t vd;
        vd.size = graph.degree(i);
        vd.addr = cl[target_cl_ch].size();
        vd.chn_id = target_cl_ch;
        vd.unused = 0;

        lvertex_desp_t  vd_device;
        vd_device = *((lvertex_desp_t *)&vd);
        rp[target_rp_ch].push_back(vd_device);

        for (auto j = 0; j < graph.degree(i); j ++)
        {
            uint64_t addr = graph.offset_[i];
            lvertex_host_t  cl_v;
            cl_v.vid = graph.adj_[addr + j];
            cl[target_cl_ch].push_back(cl_v);
        }
    }

    for (int i = 0; i < partition_number; i ++)
    {
        printf("[P%02d] rp %ld (%lf MB), cl %ld (%lf MB)\n", i,
               rp[i].size(),
               rp[i].size() * sizeof(lvertex_desp_t) / 1024.0 / 1024.0,
               cl[i].size(),
               cl[i].size() * sizeof(lvertex_host_t) / 1024.0 / 1024.0
              );
    }


    return 0;
}

#include "config_bipart.h"



#define SEED  (0xf0fffff0)
int main(int argc, char *argv[]) {
    printf("[BUILD TIME]: %s %s\n", __DATE__, __TIME__);
    //config_network(argc, argv, true);

#include "load_graph.h"

    std::vector< lvertex_desp_vector_t > rp_partition_set;
    std::vector< lvertex_host_vector_t > cl_partition_set;

    prepare_graph(graph, NUM_OF_CHN * 2, rp_partition_set, cl_partition_set);


    cl_accelerator acc_1(binaryFile, 1);

    cl_accelerator acc_2(binaryFile, 2);
    //cl_accelerator acc_2(binaryFile,2);

    uint32_t         golden_size[NUM_OF_SWITCH_STREAMS];

    query_statistic     krnl_query_statistic_1(acc_1, 0);
    query_statistic     krnl_query_statistic_2(acc_1, 1);
    query_statistic     krnl_query_statistic_3(acc_2, 0);
    query_statistic     krnl_query_statistic_4(acc_2, 1);

    rng_config_loader       krnl_query_loader_1(acc_1, 0);
    rng_config_loader       krnl_query_loader_2(acc_1, 1);
    rng_config_loader       krnl_query_loader_3(acc_2, 0);
    rng_config_loader       krnl_query_loader_4(acc_2, 1);


    uint32_t num_v = graph.num_vertices();
    unsigned r = 0;

    while (num_v >>= 1) {
        r++;
    }
    uint32_t  id_mask = ((1 << r) - 1);
    printf("0x%08x\n", id_mask);

    krnl_query_loader_1.set_config(id_map_1);
    krnl_query_loader_2.set_config(id_map_2);
    krnl_query_loader_3.set_config(id_map_3);
    krnl_query_loader_4.set_config(id_map_4);

    //uint64_t * p_query = krnl_query_loader.init_host_buffer(SIZE_OF_MERGE, QUERY_LEN);

    //for (int i = 0; i < SIZE_OF_MERGE; i ++)
    //{
    //    p_query[i] = pcg32_random() % graph.num_vertices();
    //}


#define __NUM_CONFIG__  (4)



    krnl_query_loader_1.init_rng_query(SIZE_OF_MERGE, 10);
    krnl_query_loader_2.init_rng_query(SIZE_OF_MERGE, 10);
    krnl_query_loader_3.init_rng_query(SIZE_OF_MERGE, 10);
    krnl_query_loader_4.init_rng_query(SIZE_OF_MERGE, 10);


    krnl_query_statistic_1.init();
    krnl_query_statistic_2.init();
    krnl_query_statistic_3.init();
    krnl_query_statistic_4.init();





    for (int iteration = 0; iteration < 4; iteration ++)
    {
        double start, stop;

        start = getCurrentTimestamp();
        krnl_query_loader_1.async_schedule_task();
        krnl_query_loader_2.async_schedule_task();
        krnl_query_loader_3.async_schedule_task();
        krnl_query_loader_4.async_schedule_task();
        krnl_query_loader_1.async_join();
        krnl_query_loader_2.async_join();
        krnl_query_loader_3.async_join();
        krnl_query_loader_4.async_join();
        stop = getCurrentTimestamp();

        printf("exec_time: %lf thr %lf MStep/s\n", (stop - start), SIZE_OF_MERGE * __NUM_CONFIG__   / (stop - start) / 1000 / 1000);

        sleep(1);
        krnl_query_statistic_1.async_schedule_task();
        krnl_query_statistic_1.async_join();

        krnl_query_statistic_2.async_schedule_task();
        krnl_query_statistic_2.async_join();

        krnl_query_statistic_3.async_schedule_task();
        krnl_query_statistic_3.async_join();

        krnl_query_statistic_4.async_schedule_task();
        krnl_query_statistic_4.async_join();

        krnl_query_statistic_1.get_res();
        krnl_query_statistic_2.get_res();
        krnl_query_statistic_3.get_res();
        krnl_query_statistic_4.get_res();

    }
    //dump_full_dist(true);


    return 0;
}


