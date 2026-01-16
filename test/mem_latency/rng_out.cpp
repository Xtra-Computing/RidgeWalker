
#include "latency_common.h"



extern "C" {
    void rng_out(stride_addr_stream_t &out_stream)
    {
#pragma HLS interface ap_ctrl_none port=return
        const unsigned int  size =  (1024  * 1024 * 4);
        minRand(16807, 1);
rng_loop: while(1)
        {
            stride_addr_pkg_t pkg;
#pragma HLS PIPELINE II=1
            uint32x1_t rng_data = minRand(31, 0);
            uint32x1_t addr = (rng_data % (size  / 16));
            pkg.data.ap_member(stride_addr_t, addr) = addr;
            out_stream.write(pkg);
        }
    }
}