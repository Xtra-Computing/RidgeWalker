#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "common_dummy_data.hpp"


void dummy_source_instance(STREAM_TYPE(STRUCT_NAME)   &output)
{
#pragma HLS interface ap_ctrl_none port=return
    uint64_t  counter = 0;

dummy_source: for (;;)
    {
        counter  ++;

        if (counter == 0xfffffffffffffffeULL)
        {
            PKG_TYPE(STRUCT_NAME) pkg;
            pkg.data = 0;
            pkg.last = 0;

            output.write(pkg);
        }
    }
}



extern "C" {
    void FUNC_NAME(
        STREAM_TYPE(STRUCT_NAME)   &output1,
        STREAM_TYPE(STRUCT_NAME)   &output2,
        STREAM_TYPE(STRUCT_NAME)   &output3,
        STREAM_TYPE(STRUCT_NAME)   &output4
    )
    {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow
        dummy_source_instance(output1);
        dummy_source_instance(output2);
        dummy_source_instance(output3);
        dummy_source_instance(output4);

    }
}

