#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>

#include "sink_instance.hpp"

extern "C" {
    void router_sink( ap_uint<64>                     *mem,
                      uint32_t                        num_last,
                      uint32_t                        nolast,
                      uint32_t                        max_size,
                      timeout_debug_stream_t          &in

                    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_write_outstanding=1  max_write_burst_length=2
#pragma HLS dataflow

        debug_msg_inner_stream_t debug_stream[1];

        sink_instance<timeout_debug_stream_t, timeout_debug_pkg_t> (num_last, nolast, max_size, in, debug_stream[1 - 1]);

        debug_dump < 1 > (debug_stream, mem);

    }
}

