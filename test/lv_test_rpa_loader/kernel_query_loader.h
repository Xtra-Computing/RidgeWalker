#ifndef __QUERY_LOADER_H__
#define __QUERY_LOADER_H__

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

#include "rng_type.h"
#include "rw_type.h"

typedef std::vector< uint64_t, aligned_allocator< uint64_t > > query_vector_t;




class query_loader: public cl_krnl
{
public:
    query_loader() {}
    uint32_t seed;
    uint32_t id;
    uint32_t mask;
    uint32_t size;

    cl::Buffer host_data_buffer;

    query_loader(cl_accelerator &acc, uint32_t id)
        : cl_krnl(acc, "query_loader", id)
    {
        srand( (unsigned)time(NULL) );
    }

    uint64_t*  init_host_buffer(uint32_t query_size, uint32_t query_len) {
        TRACE();
        size_t size_in_bytes = query_size * sizeof(uint64_t);
        uint32_t  num_query_x8 = query_size / 8;

        cl::Buffer buffer (context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, size_in_bytes , &host_buffer_ext);
        host_data_buffer = buffer;
        uint64_t * p_data = nullptr;
        OCL_CHECK(err, err = krnl.setArg(0, host_data_buffer));
        OCL_CHECK(err, err = krnl.setArg(1, num_query_x8));
        OCL_CHECK(err, err = krnl.setArg(2, query_len));

        OCL_CHECK(err, p_data = (uint64_t*)mem_q.enqueueMapBuffer(host_data_buffer, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, size_in_bytes,
                                nullptr, nullptr, &err));
        mem_q.finish();
        // 8 PEs



        set_init_done();

        return p_data;
    }

    int init(query_vector_t  &query, uint32_t query_len)
    {
        TRACE()
        DEBUG_PRINTF("size: %d", query.size() * sizeof(uint64_t));
        HOST_BUFFER(query, query.size() * sizeof(uint64_t));
        OCL_CHECK(err, err = krnl.setArg(0, buffer_query));

        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({   buffer_query,
                                                            }, 0 /*0 means from host*/));

        uint32_t  num_query_x8 = query.size() / 8;

        OCL_CHECK(err, err = krnl.setArg(1, num_query_x8));
        OCL_CHECK(err, err = krnl.setArg(2, query_len));

        OCL_CHECK(err, err = mem_q.finish());


        set_init_done();
        return 0;

    }
    int init(int size, int len)
    {
        DEBUG_PRINTF("dummy query for test");
        query_vector_t test_vector;

        for (int i = 0; i < size; i ++)
        {
            test_vector.push_back(i % (1024 * 4));
        }
        this->init(test_vector, len);

        return 0;
    }

    int init(int size)
    {
        this->init(size, 10);

        return 0;
    }

    int init(   uint32_t size,
                uint32_t counter_a,
                uint32_t counter_b,
                uint32_t id,
                uint32_t mask,
                uint32_t in_seed,
                uint32_t nolast)
    {
        return init(size);
    }
};

#endif  /* __QUERY_LOADER_H__ */
