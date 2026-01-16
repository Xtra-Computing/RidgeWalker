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



#include "kernel_router_state_sink.h"
#include "kernel_router_source.h"



#define SIZE_OF_ROW_LIST    (16384)

#ifdef __EMULATION__
#define SIZE_OF_MERGE       (1024 * 2)
#else
#define SIZE_OF_MERGE       (1024 * 1024 * 64 * 25)
#endif



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

#define NUM_OF_CHN  (8)

#define SEED  (0xf0fffff0)
int main(int argc, char *argv[]) {
    printf("[BUILD TIME]: %s %s\n", __DATE__, __TIME__);

#include "load_graph.h"

    std::vector< lvertex_desp_vector_t > rp_partition_set;
    std::vector< lvertex_host_vector_t > cl_partition_set;

    //prepare_graph(graph, NUM_OF_CHN * 2, rp_partition_set, cl_partition_set);


    cl_accelerator acc(binaryFile);


    router_sink      krnl_router_sink(acc, 0);
    router_source    krnl_router_source(acc, 0);



    krnl_router_source.init(SIZE_OF_MERGE, 1024, 0 ,
                            0,  0xf, SEED , true);
    krnl_router_sink.init();

    for (int iteration = 0; iteration < 3; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();



        krnl_router_source.async_schedule_task();
        krnl_router_source.async_join();



        stop = getCurrentTimestamp();
        printf("exec_time: %lf thr %lf MStep/s\n", (stop - start), SIZE_OF_MERGE / (stop - start) / 1000 / 1000);
        krnl_router_sink.async_schedule_task();
        krnl_router_sink.async_join();
        krnl_router_sink.async_schedule_task();
        krnl_router_sink.async_join();

        krnl_router_sink.get_res();

    }

    return 0;
}


