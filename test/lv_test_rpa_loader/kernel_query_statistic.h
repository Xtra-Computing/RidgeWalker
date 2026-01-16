#ifndef __QUERY_STATISTIC_H__
#define __QUERY_STATISTIC_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>

#include "pcg_basic.h"

#include "helper.h"
#include "host_test_common.h"
#include "fpga_kernel.h"



class query_statistic: public cl_krnl
{
public:
    uint32_t seed;
    uint32_t size_of_sink;

    cl::Buffer res_buffer;

    cl::Buffer host_data_buffer;


    std::vector<uint64_t, aligned_allocator<uint64_t> > data_array;


    query_statistic() {}

    query_statistic(cl_accelerator &acc, uint32_t id)
        : cl_krnl(acc, "query_statistic", id)
    {
        srand( (unsigned)time(NULL) );
    }

    int init(void)
    {
        size_of_sink = 64;


        for (uint32_t i = 0; i < size_of_sink; i ++) {
            data_array.push_back(0);
        }

        return mem_transfer(data_array);
    }

     uint64_t*  init_host_buffer(void) {
        TRACE();
        size_t size_in_bytes = 64 * sizeof(uint64_t);
        cl::Buffer buffer (context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, size_in_bytes , &host_buffer_ext);
        host_data_buffer = buffer;
        uint64_t * p_data = nullptr;
        OCL_CHECK(err, err = krnl.setArg(0, host_data_buffer));

        OCL_CHECK(err, p_data = (uint64_t*)mem_q.enqueueMapBuffer(host_data_buffer, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, size_in_bytes,
                              nullptr, nullptr, &err));
        mem_q.finish();




        set_init_done();

        return p_data;
    }




    int mem_transfer(   std::vector<uint64_t, aligned_allocator<uint64_t> >  & data)
    {
        TRACE()
        HOST_BUFFER(data, size_of_sink * sizeof(uint64_t));
        res_buffer = buffer_data;
        OCL_CHECK(err, err = krnl.setArg(0, buffer_data));

        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({   buffer_data
                                                            }, 0 /*0 means from host*/));
        OCL_CHECK(err, err = mem_q.finish());
        set_init_done();
        return 0;
    }


    uint64_t get_res(void)
    {
        TRACE()

        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({res_buffer}, CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = mem_q.finish());
        printf("res 0x%016lx\n", data_array[0]);


        return data_array[15];
    }

    int get_res(int num_chn)
    {
        TRACE()

        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({res_buffer}, CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = mem_q.finish());
        for (int i = 0; i < num_chn ; i ++)
        {
            printf("[%d] size %ld\n", i, data_array[i]);
        }

        for (int i = 0; i < num_chn ; i ++)
        {
            printf("[%d] res 0x%016lx\n", i, data_array[i + num_chn]);
        }
        return 0;
    }
};

#endif  /* __QUERY_STATISTIC_H__ */
