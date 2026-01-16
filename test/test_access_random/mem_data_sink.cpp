#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rng_type.h"

#include <ap_int.h>

#ifndef MEM_INTERFACE_WIDTH
#define BUS_WIDTH  (256)
#else
#define BUS_WIDTH  (MEM_INTERFACE_WIDTH)
#endif

extern "C" {
    void mem_data_sink (
        ap_uint< BUS_WIDTH >                *res,
        mem_access_stream_t(BUS_WIDTH)    &in_data
    ) {

#pragma HLS INTERFACE m_axi port = res offset = slave bundle = gmem1 // latency = 64

#pragma HLS INTERFACE s_axilite port = res bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

        ap_uint< BUS_WIDTH > output = 0;

        while (1)
        {
#pragma HLS PIPELINE II=1
            mem_access_pkg_t(BUS_WIDTH) data_pkg = in_data.read();

            output += data_pkg.data.range(BUS_WIDTH - 1, 0);
            if (data_pkg.last == 1)
            {
                break;
            }
        }
        res[0] = output;
    }
}
