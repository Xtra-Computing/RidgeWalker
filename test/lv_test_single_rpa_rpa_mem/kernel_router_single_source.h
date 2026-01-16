#ifndef __ROUTER_SOURCE_H__
#define __ROUTER_SOURCE_H__

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




class router_source: public cl_krnl
{
public:
    router_source() {}


    router_source(cl_accelerator &acc, uint32_t id)
        : cl_krnl(acc, "router_source", id)
    {
        srand( (unsigned)time(NULL) );
    }

    int mem_transfer(   std::vector<uint128_t, aligned_allocator<uint128_t> >  & data)
    {
        TRACE()
        HOST_BUFFER(data, data.size() * sizeof(uint128_t));
        OCL_CHECK(err, err = krnl.setArg(0, buffer_data));
        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({   buffer_data
                                                            }, 0 /*0 means from host*/));
        OCL_CHECK(err, err = mem_q.finish());

        return 0;
    }

    int init(std::vector<uint128_t, aligned_allocator<uint128_t> >  & data)
    {
        uint32_t counter_a = 512;
        uint32_t counter_b = 0;
        this->init(counter_a, counter_b, data);
        return 0;
    }


    int init( uint32_t counter_a, uint32_t counter_b,  std::vector<uint128_t, aligned_allocator<uint128_t> >  & data)
    {
        uint32_t size = data.size();
        mem_transfer(data);

        TRACE();
        OCL_CHECK(err, err = krnl.setArg(1, size));
        OCL_CHECK(err, err = krnl.setArg(2, counter_a));
        OCL_CHECK(err, err = krnl.setArg(3, counter_b));
        set_init_done();
        return 0;
    }


};

#endif  /* __ROUTER_SOURCE_H__ */
