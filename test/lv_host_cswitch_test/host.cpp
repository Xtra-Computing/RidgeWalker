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
#include "kernel_rng_config_loader.h"
#include "kernel_config_loader.h"





#define SIZE_OF_ROW_LIST    (16384)

#ifdef __EMULATION__
#define SIZE_OF_MERGE       (1024 * 64)
#else
#define SIZE_OF_MERGE       (1024 * 1024 * 16)
#endif

#define NUM_OF_CHN  (8)

#define QUERY_LEN   (1)


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

router_mapper_config_t id_map[] = {
    {
        {
            {.ori_id = 0, .mapped_id = 0},
            {.ori_id = 1, .mapped_id = 0},
            {.ori_id = 2, .mapped_id = 0},
            {.ori_id = 3, .mapped_id = 0},
            {.ori_id = 4, .mapped_id = 0},
            {.ori_id = 5, .mapped_id = 0},
            {.ori_id = 6, .mapped_id = 0},
            {.ori_id = 7, .mapped_id = 0},
        },
    },
};


#define SEED  (0xf0fffff0)
int main(int argc, char *argv[]) {
    printf("[BUILD TIME]: %s %s\n", __DATE__, __TIME__);

#include "load_graph.h"

    std::vector< lvertex_desp_vector_t > rp_partition_set;
    std::vector< lvertex_host_vector_t > cl_partition_set;

    prepare_graph(graph, NUM_OF_CHN * 2, rp_partition_set, cl_partition_set);


    cl_accelerator acc(binaryFile);
    uint32_t         golden_size[NUM_OF_SWITCH_STREAMS];

    query_statistic     krnl_query_statistic_1(acc, 0);
    query_statistic     krnl_query_statistic_2(acc, 1);
    rng_config_loader       krnl_query_loader_1(acc, 0);
    rng_config_loader       krnl_query_loader_2(acc, 1);


    uint32_t num_v = graph.num_vertices();
    unsigned r = 0;

    while (num_v >>= 1) {
        r++;
    }
    uint32_t  id_mask = ((1 << r) - 1);
    printf("0x%08x\n", id_mask);

    krnl_query_loader_1.set_config(id_map);
    krnl_query_loader_2.set_config(id_map);

    //uint64_t * p_query = krnl_query_loader.init_host_buffer(SIZE_OF_MERGE, QUERY_LEN);

    //for (int i = 0; i < SIZE_OF_MERGE; i ++)
    //{
    //    p_query[i] = pcg32_random() % graph.num_vertices();
    //}


#define __NUM_CONFIG__  (2)

    query_vector_t  queries[__NUM_CONFIG__];


    for (int j = 0; j < __NUM_CONFIG__; j ++)
    {
        uint32_t counter = 0;
        for (int i = 0; i < SIZE_OF_MERGE; i ++)
        {
            uint32_t l_data = pcg32_random();

            uint64_t data = l_data;

            queries[j].push_back(data);

            uint8_t ori_id = ((data >> 29) & 0x7);
            if (id_map->item[ori_id].mapped_id == 0) {
                counter ++;
            }
        }
        printf("Q[%d] groundtruth: local 0x%x, remote 0x%x\n",j, counter, SIZE_OF_MERGE - counter);
    }



    krnl_query_loader_1.init_rng_query(queries[0], 10);
    krnl_query_loader_2.init_rng_query(queries[1], 10);


    krnl_query_statistic_1.init();
    krnl_query_statistic_2.init();





    for (int iteration = 0; iteration < 4; iteration ++)
    {
        double start, stop;

        start = getCurrentTimestamp();
        krnl_query_loader_1.async_schedule_task();
        krnl_query_loader_2.async_schedule_task();
        krnl_query_loader_1.async_join();
        krnl_query_loader_2.async_join();
        stop = getCurrentTimestamp();

        printf("exec_time: %lf thr %lf MStep/s\n", (stop - start), SIZE_OF_MERGE  / (stop - start) / 1000 / 1000);

        krnl_query_statistic_1.async_schedule_task();
        krnl_query_statistic_1.async_join();

        krnl_query_statistic_2.async_schedule_task();
        krnl_query_statistic_2.async_join();

        krnl_query_statistic_1.get_res();
        krnl_query_statistic_2.get_res();

    }

    return 0;
}


