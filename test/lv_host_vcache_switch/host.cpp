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



#include "kernel_router_sink.h"
#include "kernel_router_source.h"
#include "kernel_vcache_mem.h"
#include "end_ctrl.h"


#define SIZE_OF_ROW_LIST    (16384)

#ifdef __EMULATION__
#define SIZE_OF_MERGE       (1024)
#else
#define SIZE_OF_MERGE       (1024 * 1024 * 64)
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



    for (auto i = 0; i < graph.num_vertices(); i++) {

        // shuffle, shift 1 for debug
        uint32_t target_cl_ch =  (i) & (partition_number - 1);

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
            rp[i].size() * sizeof(lvertex_desp_t)/1024.0/1024.0,
            cl[i].size(),
            cl[i].size() * sizeof(lvertex_host_t)/1024.0/1024.0
            );
    }


    return 0;
}

#define NUM_OF_CHN  (8)

#define SEED  (0xffffff)
int main(int argc, char *argv[]) {


#include "load_graph.h"

    std::vector< lvertex_desp_vector_t > rp_partition_set;
    std::vector< lvertex_host_vector_t > cl_partition_set;

    prepare_graph(graph, NUM_OF_CHN * 2,rp_partition_set, cl_partition_set);


    cl_accelerator acc(binaryFile);
    uint32_t         golden_size[NUM_OF_SWITCH_STREAMS];

    router_sink      krnl_router_sink(acc, 0);
    router_source    krnl_router_source(acc, 0);
    end_ctrl         krnl_end_ctrl(acc, 0);
    vcache_mem       krnl_vcache_mem[2];

    uint32_t num_v = graph.num_vertices();
    unsigned r = 0;

    while (num_v >>= 1) {
         r++;
    }
    uint32_t  id_mask = ((1 << r) - 1);
    printf("0x%08x\n", id_mask);


    krnl_router_source.init(SIZE_OF_MERGE, 1024, 0 ,
                            0,  id_mask, SEED , true);
    krnl_router_sink.init(0, true);


    for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++) {
        golden_size[i] =  0;
    }


    for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++) {
        printf("[%d] set %d \n", i, golden_size[i]);
        krnl_router_sink.set_size(i, golden_size[i]);
    }

    for (int j = 0; j < 2 ; j ++)
    {
        krnl_vcache_mem[j] =  vcache_mem(acc, j, 0);
        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++)
        {
            krnl_vcache_mem[j].init(rp_partition_set[j * NUM_OF_SWITCH_STREAMS + i], i);
        }
    }

    for (int iteration = 0; iteration < 2; iteration ++)
    {
        double start, stop;
        start = getCurrentTimestamp();

        for (int j = 0; j < 2 ; j ++) {
            krnl_vcache_mem[j].async_schedule_task();
        }


        krnl_router_source.async_schedule_task();
        krnl_router_sink.async_schedule_task();
        krnl_router_sink.async_join();
        krnl_router_source.async_join();

        krnl_end_ctrl.init(CTRL_END_OF_STREAM);
        krnl_end_ctrl.async_schedule_task();
        krnl_end_ctrl.async_join();


        for (int j = 0; j < 2 ; j ++) {
            krnl_vcache_mem[j].async_join();
        }
        stop = getCurrentTimestamp();
        printf("exec_time: %lf thr %lf MStep/s\n", (stop - start), SIZE_OF_MERGE / (stop - start) / 1000 / 1000);
    }

    krnl_router_sink.get_res();
    return 0;
}


