#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"

#include "sink_instance.hpp"

#define  PERFORMANCE_TEST (1)




extern "C" {
    void FUNC_NAME( ap_uint<64>                     *mem,
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

                    STREAM_TYPE(name)       &in1,
                    STREAM_TYPE(name)       &in2,
                    STREAM_TYPE(name)       &in3,
                    STREAM_TYPE(name)       &in4,
                    STREAM_TYPE(name)       &in5,
                    STREAM_TYPE(name)       &in6,
                    STREAM_TYPE(name)       &in7,
                    STREAM_TYPE(name)       &in8

                  )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_write_outstanding=1  max_write_burst_length=2
#pragma HLS dataflow

        debug_msg_inner_stream_t debug_stream[8];


        sink_instance(num_last, nolast, max_size1, in1, debug_stream[1 - 1]);
        sink_instance(num_last, nolast, max_size2, in2, debug_stream[2 - 1]);
        sink_instance(num_last, nolast, max_size3, in3, debug_stream[3 - 1]);
        sink_instance(num_last, nolast, max_size4, in4, debug_stream[4 - 1]);
        sink_instance(num_last, nolast, max_size5, in5, debug_stream[5 - 1]);
        sink_instance(num_last, nolast, max_size6, in6, debug_stream[6 - 1]);
        sink_instance(num_last, nolast, max_size7, in7, debug_stream[7 - 1]);
        sink_instance(num_last, nolast, max_size8, in8, debug_stream[8 - 1]);

        debug_dump < 8 > (debug_stream, mem);

    }
}

