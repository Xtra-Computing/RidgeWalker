#ifndef __RPA_ENGINE_H__
#define __RPA_ENGINE_H__

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

#include "rw_type.h"




class rpa_engine: public cl_krnl
{

public:
    rpa_engine() {}

    rpa_engine(cl_accelerator &acc, uint32_t gid, uint32_t id)
        : cl_krnl(acc, "rpa_engine", gid, id, 1)
    {

    }


    int init(lvertex_desp_vector_t  &row_index, uint32_t arg_index)
    {
        TRACE()
        HOST_BUFFER(row_index, row_index.size() * sizeof(lvertex_desp_t));
        OCL_CHECK(err, err = krnl.setArg(arg_index, buffer_row_index));

        OCL_CHECK(err, err = mem_q.enqueueMigrateMemObjects({   buffer_row_index,
                                                            }, 0 /*0 means from host*/));
        OCL_CHECK(err, err = mem_q.finish());

        set_init_done();
        return 0;

    }
};




#endif /* __RPA_ENGINE_H__ */
