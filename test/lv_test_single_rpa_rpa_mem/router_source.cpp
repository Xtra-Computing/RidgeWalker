#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>

#include "rng_type.h"
#include "rw_type.h"





extern "C" {
    void router_source(
        ap_uint<128>            *mem,
        uint32_t                size,
        uint32_t                counter_a,
        uint32_t                counter_b,
        rpa_task_stream_t       &output
    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_read_outstanding=256  max_read_burst_length=32


        for (int j = 0; j < size; j += 32) {
            for (int i = 0; i < 32; i ++) {
#pragma HLS pipeline
                ap_uint<128> mdata = mem[j + i];
                rpa_task_pkg_t pkg;
                pkg.data = mdata;
                //pkg.data.range(15,0) = j + i;
                pkg.last = ((j + i) == (size - 1));
                output.write(pkg);
            }
        }

    }
}

