#ifndef __MEM_ACCESS_SIM_H__
#define __MEM_ACCESS_SIM_H__


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



class mem_access_sim: public cl_krnl
{
public:
    uint32_t seed;
    uint32_t num_of_access;

    mem_access_sim() {}

    mem_access_sim(cl_accelerator &acc, uint32_t id)
        : cl_krnl(acc, "mem_access_sim", id)
    {
        srand( (unsigned)time(NULL) );
    }

    int init(uint32_t  num_of_access, uint32_t counter_a,uint32_t counter_b, uint32_t mask)
    {
        this->init(num_of_access, counter_a, counter_b, mask, 1);
        return 0;
    }

    int init(uint32_t  num_of_access, uint32_t counter_a,uint32_t counter_b, uint32_t mask, uint32_t last)
    {
        TRACE()
        seed = rand();
        this->num_of_access = num_of_access;
        uint32_t in_num_of_access = num_of_access;
        uint32_t in_seed = seed;
        OCL_CHECK(err, err = krnl.setArg(0, in_num_of_access));
        OCL_CHECK(err, err = krnl.setArg(1, in_seed));
        OCL_CHECK(err, err = krnl.setArg(2, counter_a));
        OCL_CHECK(err, err = krnl.setArg(3, counter_b));
        OCL_CHECK(err, err = krnl.setArg(4, mask));
        OCL_CHECK(err, err = krnl.setArg(5, last));
        set_init_done();
        return 0;
    }

    uint32_t get_num_of_access(void)
    {
        return this->num_of_access;
    }
};

#endif  /* __MEM_ACCESS_SIM_H__ */
