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



#include "kernel_query_statistic.h"
#include "kernel_config_loader.h"


#include "kernel_rpa_engine.h"
#include "kernel_cla_engine.h"
#include "end_ctrl.h"

#include "switch_data_struct.h"


#define SIZE_OF_ROW_LIST    (16384)

#ifdef __EMULATION__
#define SIZE_OF_MERGE       (1024 * 64)
#else
#define SIZE_OF_MERGE       (1024 * 1024 * 16)
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

router_mapper_config_t rpa_map[] = {
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

router_mapper_config_t cla_map[] = {
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

    query_statistic     krnl_query_statistic(acc, 0);
    config_loader       krnl_query_loader(acc, 0);


    end_ctrl         krnl_end_ctrl_rpa(acc, 0);
    end_ctrl         krnl_end_ctrl_cla(acc, 1);

    rpa_engine       krnl_rpa_engine[2];
    cla_engine       krnl_cla_engine[2];

    uint32_t num_v = graph.num_vertices();
    unsigned r = 0;

    while (num_v >>= 1) {
        r++;
    }
    uint32_t  id_mask = ((1 << r) - 1);
    printf("0x%08x\n", id_mask);
    krnl_query_loader.set_rpa_mapper_from_host(rpa_map);
    krnl_query_loader.set_cla_mapper_from_host(cla_map);


    uint64_t * p_query = krnl_query_loader.init_host_buffer(SIZE_OF_MERGE, QUERY_LEN);


    for (int i = 0; i < SIZE_OF_MERGE; i ++)
    {
        p_query[i] = pcg32_random() % graph.num_vertices();
    }



    uint64_t * p_res = krnl_query_statistic.init_host_buffer();


    krnl_query_statistic.async_schedule_task();
    krnl_query_statistic.async_join();
    printf("res: %lu\n",    p_res[0]);


    for (int j = 0; j < 2 ; j ++)
    {
        krnl_rpa_engine[j] =  rpa_engine(acc, j, 0);
        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++)
        {
            krnl_rpa_engine[j].init(rp_partition_set[i * 2 + j], i);
        }
    }


    for (int j = 0; j < 2 ; j ++)
    {
        krnl_cla_engine[j] =  cla_engine(acc, j, 0);
        for (int i = 0; i < NUM_OF_SWITCH_STREAMS; i ++)
        {
            krnl_cla_engine[j].init(cl_partition_set[i * 2 + j], i);
        }
    }


    for (int iteration = 0; iteration < 4; iteration ++)
    {
        double start, stop;
        for (int j = 0; j < 2 ; j ++) {
            krnl_rpa_engine[j].async_schedule_task();
            krnl_cla_engine[j].async_schedule_task();
        }

        start = getCurrentTimestamp();


        //sleep(10);
        //sleep(1);
        krnl_query_loader.async_schedule_task();


        krnl_query_loader.async_join();
        stop = getCurrentTimestamp();
        printf("exec_time: %lf thr %lf MStep/s\n", (stop - start), SIZE_OF_MERGE * (QUERY_LEN + 1) / (stop - start) / 1000 / 1000);

        krnl_query_statistic.async_schedule_task();
        //sleep(1);
        krnl_query_statistic.async_join();
        printf("res: %lu\n",    p_res[0]);







        //sleep (1);
        //while (1)
        {
            //krnl_query_statistic.async_schedule_task();
            //krnl_query_statistic.async_join();

            //uint64_t res = krnl_query_statistic.get_res();
            //if ((res % ((QUERY_LEN + 1) *SIZE_OF_MERGE )) == 0 ) {
            //   // break;
            //}
        }





        krnl_end_ctrl_rpa.init(CTRL_END_OF_STREAM);
        krnl_end_ctrl_rpa.async_schedule_task();
        krnl_end_ctrl_rpa.async_join();


        krnl_end_ctrl_cla.init(CTRL_END_OF_STREAM);
        krnl_end_ctrl_cla.async_schedule_task();
        krnl_end_ctrl_cla.async_join();


        for (int j = 0; j < 2 ; j ++) {
            krnl_rpa_engine[j].async_join();
            krnl_cla_engine[j].async_join();
        }
    }

    return 0;
}


