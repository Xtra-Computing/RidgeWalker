#ifndef __ROUTER_SINK_H__
#define __ROUTER_SINK_H__

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

#include "stream_type.h"



class router_sink: public cl_krnl
{
public:


    cl::Buffer res_buffer;

    std::vector<uint128_t, aligned_allocator<uint128_t> > data_array;


    router_sink() {}

    router_sink(cl_accelerator &acc, uint32_t id)
        : cl_krnl(acc, "router_sink", id)
    {
        srand( (unsigned)time(NULL) );
    }



    int init( uint32_t size)
    {

        data_array.clear();
        for (uint32_t i = 0; i < size; i ++) {
            uint128_t item = {0};
            data_array.push_back(item);
        }
        return mem_transfer(data_array);
    }

    int mem_transfer(  std::vector<uint128_t, aligned_allocator<uint128_t> >  & data)
    {
        int size_of_item = data.size();
        TRACE()
        HOST_BUFFER(data, data.size() * sizeof(uint128_t));
        res_buffer = buffer_data;
        OCL_CHECK(err, err = krnl.setArg(0, buffer_data));
        OCL_CHECK(err, err = krnl.setArg(1, size_of_item));

        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({   buffer_data
                                                            }, 0 /*0 means from host*/));
        OCL_CHECK(err, err = mem_q.finish());
        set_init_done();
        return 0;
    }




    int get_res(void)
    {
        TRACE()

        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({res_buffer}, CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = mem_q.finish());
        for (int i = 0; i < 8 ; i ++)
        {
            printf("[%d] size %ld\n", i, data_array[i]);
        }

        for (int i = 0; i < 8 ; i ++)
        {
            printf("[%d] res 0x%016lx\n", i, data_array[i + 8]);
        }
        return 0;
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

#endif  /* __ROUTER_SINK_H__ */
