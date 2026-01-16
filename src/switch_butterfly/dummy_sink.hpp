#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "common_dummy_data.hpp"



void dummy_sink_instance(STREAM_TYPE(STRUCT_NAME) &in)
{
#pragma HLS interface ap_ctrl_none port=return

    while (1)
    {
        PKG_TYPE(STRUCT_NAME) pkg;
        if (in.read_nb(pkg)) {

            if (pkg.last == 1)
            {
                break;
            }
        }
    }
}


extern "C" {
    void FUNC_NAME(
        STREAM_TYPE(STRUCT_NAME)      &in1,
        STREAM_TYPE(STRUCT_NAME)      &in2,
        STREAM_TYPE(STRUCT_NAME)      &in3,
        STREAM_TYPE(STRUCT_NAME)      &in4
    )
    {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow


        dummy_sink_instance(in1);
        dummy_sink_instance(in2);
        dummy_sink_instance(in3);
        dummy_sink_instance(in4);

    }
}

