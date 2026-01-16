#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"





extern "C" {
    void rpa_ws_scheduler(
        rpa_task_stream_t    &in1,
        rpa_task_stream_t    &in2,
        rpa_task_stream_t    &in3,
        rpa_task_stream_t    &in4,
        rpa_task_stream_t    &in5,
        rpa_task_stream_t    &in6,
        rpa_task_stream_t    &in7,
        rpa_task_stream_t    &in8,

        rpa_task_stream_t    &out1,
        rpa_task_stream_t    &out2,
        rpa_task_stream_t    &out3,
        rpa_task_stream_t    &out4,
        rpa_task_stream_t    &out5,
        rpa_task_stream_t    &out6,
        rpa_task_stream_t    &out7,
        rpa_task_stream_t    &out8

    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        rpa_task_frp_inner_stream_t     in_stream[8];
#pragma HLS stream variable=in_stream  depth=13

        rpa_task_frp_inner_stream_t     out_stream[8];
#pragma HLS stream variable=out_stream  depth=13

        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in1, in_stream[0]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in2, in_stream[1]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in3, in_stream[2]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in4, in_stream[3]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in5, in_stream[4]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in6, in_stream[5]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in7, in_stream[6]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(rpa_task) >(in8, in_stream[7]);

        work_stealing_8x8_frp< FRP_TEMPLATE(rpa_task) >(
            in_stream[0], in_stream[1], in_stream[2], in_stream[3], in_stream[4], in_stream[5], in_stream[6], in_stream[7],
            out_stream[0], out_stream[1], out_stream[2], out_stream[3], out_stream[4], out_stream[5], out_stream[6], out_stream[7]
        );

        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[0], out1);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[1], out2);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[2], out3);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[3], out4);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[4], out5);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[5], out6);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[6], out7);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(rpa_task) >(out_stream[7], out8);


    }
}

