#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"


extern "C" {
    void router_sink( ap_uint<64>                     *mem,
                      debug_msg_stream_t              &state
                    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_write_outstanding=1  max_write_burst_length=2

        for (int i = 0 ; i < 16 ; i ++)
        {
            debug_msg_pkg_t pkg = state.read();
            mem[i] = pkg.data;
        }

    }
}

