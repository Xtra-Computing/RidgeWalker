#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"

#include "sink_instance.hpp"





extern "C" {
    void router_sink( ap_uint<64>                     *mem,
                      uint32_t                        num_last,
                      uint32_t                        nolast,
                      uint32_t                        max_size1,
                      uint32_t                        max_size2,
                      uint32_t                        max_size3,
                      uint32_t                        max_size4,
                      uint32_t                        max_size5,
                      uint32_t                        max_size6,
                      uint32_t                        max_size7,
                      uint32_t                        max_size8,

                      cla_task_stream_t      &in1,
                      cla_task_stream_t      &in2,
                      cla_task_stream_t      &in3,
                      cla_task_stream_t      &in4


                    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_write_outstanding=1  max_write_burst_length=2
#pragma HLS dataflow

        debug_msg_inner_stream_t debug_stream[4];

        sink_instance<cla_task_stream_t, cla_task_pkg_t>(num_last, nolast, max_size1, in1, debug_stream[1 - 1]);
        sink_instance<cla_task_stream_t, cla_task_pkg_t>(num_last, nolast, max_size2, in2, debug_stream[2 - 1]);
        sink_instance<cla_task_stream_t, cla_task_pkg_t>(num_last, nolast, max_size3, in3, debug_stream[3 - 1]);
        sink_instance<cla_task_stream_t, cla_task_pkg_t>(num_last, nolast, max_size4, in4, debug_stream[4 - 1]);

        debug_dump <4> (debug_stream, mem);

    }
}

