#ifndef __X4_ROUTER_SINK_H__
#define __X4_ROUTER_SINK_H__

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



class x4_router_sink: public cl_krnl
{
public:
    uint32_t seed;
    uint32_t size_of_sink;

    cl::Buffer res_buffer;

    std::vector<uint64_t, aligned_allocator<uint64_t> > data_array;


    x4_router_sink() {}

    x4_router_sink(cl_accelerator &acc, uint32_t id)
        : cl_krnl(acc, "x4_router_sink", id)
    {
        srand( (unsigned)time(NULL) );
    }

    int init( uint32_t num_last, uint32_t nolast)
    {
        size_of_sink = 1024;

        OCL_CHECK(err, err = krnl.setArg(1, num_last));
        OCL_CHECK(err, err = krnl.setArg(2, nolast));

        data_array.clear();

        for (uint32_t i = 0; i < size_of_sink; i ++){
            data_array.push_back(0);
        }

        return mem_transfer(data_array);
    }

    int init( uint32_t num_last, uint32_t nolast, uint32_t max_size)
    {
        size_of_sink = 1024;

        OCL_CHECK(err, err = krnl.setArg(1, num_last));
        OCL_CHECK(err, err = krnl.setArg(2, nolast));
        OCL_CHECK(err, err = krnl.setArg(3, max_size));

        data_array.clear();
        for (uint32_t i = 0; i < size_of_sink; i ++){
            data_array.push_back(0);
        }
        return mem_transfer(data_array);
    }

    int set_size(int i, uint32_t max_size)
    {
#ifdef __EMULATION__
        uint32_t timeout_counter = 16384 * 4;
        if (max_size >16384 * 4){
            timeout_counter = max_size;
        }
#else
        uint32_t timeout_counter = 16384 * 128 * 8;
#endif
        DEBUG_PRINTF("using %d instead of %d",timeout_counter, max_size);

        OCL_CHECK(err, err = krnl.setArg(3 + i, timeout_counter));

        return 0;
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


     int get_res(void)
    {
        TRACE()

        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({res_buffer}, CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = mem_q.finish());
        for (int i = 0; i < 8 ; i ++)
        {
            printf("[%d] size %ld\n",i, data_array[i]);
        }

        for (int i = 0; i < 8 ; i ++)
        {
            printf("[%d] res 0x%016lx\n",i, data_array[i + 8]);
        }


        return 0;
    }
};

#endif  /* __X4_ROUTER_SINK_H__ */
