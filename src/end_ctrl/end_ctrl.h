#ifndef __END_CTRL_H__
#define __END_CTRL_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>


#include "helper.h"
#include "host_test_common.h"
#include "fpga_kernel.h"
#include "stream_type.h"


class end_ctrl: public cl_krnl
{
public:
    uint8_t ctrl;

    end_ctrl() {}

    end_ctrl(cl_accelerator &acc, uint32_t id)
        : cl_krnl(acc, "end_ctrl", id)
    {

    }

    int init(uint8_t ctrl)
    {
        TRACE()
        this->ctrl = ctrl;
        OCL_CHECK(err, err = krnl.setArg(0,  this->ctrl));
        set_init_done();
        return 0;
    }
};

#endif  /* __END_CTRL_H__ */
