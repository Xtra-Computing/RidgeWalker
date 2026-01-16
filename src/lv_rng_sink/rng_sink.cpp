#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rng_type.h"
#include "init_state.h"

#include "stream_operation.h"

#include "sou_function.h"



extern "C" {
    void rng_sink( rng_output_stream_t       &rng)
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        sink_stream(rng);

    }
}
