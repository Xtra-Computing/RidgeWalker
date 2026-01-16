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

#include "rng_type.h"



class router_source: public cl_krnl
{
public:
    router_source() {}
    uint32_t seed;
    uint32_t id;
    uint32_t mask;
    uint32_t size;

    router_source(cl_accelerator &acc, uint32_t id)
        : cl_krnl(acc, "router_source", id)
    {
        srand( (unsigned)time(NULL) );
    }

    uint32_t simulate_size(uint32_t seed, uint32_t  target_id, uint32_t mask)
    {
        uint32_t counter = 0;
        minRand(seed, 1);
        for (uint32_t  i = 0; i <  this->size; i ++)
        {
            uint32_t rng = minRand(31, 0);

            if ((rng & mask) == target_id)
            {
                counter++;
            }
        }
        printf("To id %d has %d packets\n", target_id, counter);
        return counter;
    }

    uint32_t simulate_size(uint32_t  target_id, uint32_t mask)
    {
        return simulate_size(this->seed, target_id, mask);
    }

    uint32_t simulate_size(uint32_t  target_id)
    {
        return simulate_size(this->seed, target_id, this->mask);
    }

    uint32_t simulate_rng(void)
    {

        minRand(this->seed, 1);
        for (uint32_t  i = 0; i <  this->size; i ++)
        {
            uint32_t rng = minRand(31, 0);

            printf("[%d] 0x%08x \n", i, rng);
        }

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
        this->seed = in_seed;
        this->id = id;
        this->mask = mask;
        this->size = size;
        TRACE()
        OCL_CHECK(err, err = krnl.setArg(0, size));
        OCL_CHECK(err, err = krnl.setArg(1, counter_a));
        OCL_CHECK(err, err = krnl.setArg(2, counter_b));
        OCL_CHECK(err, err = krnl.setArg(3, id));
        OCL_CHECK(err, err = krnl.setArg(4, mask));
        OCL_CHECK(err, err = krnl.setArg(5, in_seed));
        OCL_CHECK(err, err = krnl.setArg(6, nolast));
        set_init_done();
        return 0;
    }
};

#endif  /* __ROUTER_SOURCE_H__ */
