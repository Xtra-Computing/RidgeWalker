#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rng_type.h"

#include <ap_int.h>


extern "C" {
    void mem_data_sink (
        ap_uint<64>                *res,
        mem_access_stream_t(64)    &in_data
    ) {

#pragma HLS INTERFACE m_axi port = res offset = slave bundle = gmem1 // latency = 64

#pragma HLS INTERFACE s_axilite port = res bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

        ap_uint<64> output = 0;

        while (1)
        {
#pragma HLS PIPELINE II=1
            mem_access_pkg_t(64) data_pkg = in_data.read();

            output += data_pkg.data.ap_mem_data(64);
            if (data_pkg.last == 1)
            {
                break;
            }
        }
        res[0] = output;
    }
}
