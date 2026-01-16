#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"





extern "C" {
    void rpa_ws_scheduler(
        rpa_task_stream_t    &in1,
        rpa_task_stream_t    &in2,

        rpa_task_stream_t    &out1,
        rpa_task_stream_t    &out2


    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        rpa_task_frp_inner_stream_t     in_stream[2];
#pragma HLS stream variable=in_stream  depth=13

        rpa_task_frp_inner_stream_t     out_stream[2];
#pragma HLS stream variable=out_stream  depth=13

        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in1, in_stream[0]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in2, in_stream[1]);


        work_stealing_2x2_frp< FRP_TEMPLATE(rpa_task) >(
            in_stream[0], in_stream[1],
            out_stream[0], out_stream[1]
        );

        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[0], out1);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[1], out2);



    }
}

