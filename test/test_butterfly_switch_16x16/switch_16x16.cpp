#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>



extern "C" {
    void switch_16x16(
        default_route_stream_t   &in01,
        default_route_stream_t   &in02,
        default_route_stream_t   &in03,
        default_route_stream_t   &in04,
        default_route_stream_t   &in05,
        default_route_stream_t   &in06,
        default_route_stream_t   &in07,
        default_route_stream_t   &in08,
        default_route_stream_t   &in11,
        default_route_stream_t   &in12,
        default_route_stream_t   &in13,
        default_route_stream_t   &in14,
        default_route_stream_t   &in15,
        default_route_stream_t   &in16,
        default_route_stream_t   &in17,
        default_route_stream_t   &in18,

        default_route_stream_t   &out01,
        default_route_stream_t   &out02,
        default_route_stream_t   &out03,
        default_route_stream_t   &out04,
        default_route_stream_t   &out05,
        default_route_stream_t   &out06,
        default_route_stream_t   &out07,
        default_route_stream_t   &out08,
        default_route_stream_t   &out11,
        default_route_stream_t   &out12,
        default_route_stream_t   &out13,
        default_route_stream_t   &out14,
        default_route_stream_t   &out15,
        default_route_stream_t   &out16,
        default_route_stream_t   &out17,
        default_route_stream_t   &out18


    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        default_route_frp_inner_stream_t     in_stream_l1[8];
#pragma HLS stream variable=in_stream_l1  depth=2

        default_route_frp_inner_stream_t     in_stream_l2[8];
#pragma HLS stream variable=in_stream_l2  depth=2


        default_route_frp_inner_stream_t     out_stream_l1[8];
#pragma HLS stream variable=out_stream_l1  depth=2

        default_route_frp_inner_stream_t     out_stream_l2[8];
#pragma HLS stream variable=out_stream_l2  depth=2

        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in01, in_stream_l1[0]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in02, in_stream_l1[1]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in03, in_stream_l1[2]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in04, in_stream_l1[3]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in05, in_stream_l1[4]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in06, in_stream_l1[5]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in07, in_stream_l1[6]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in08, in_stream_l1[7]);

        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in11, in_stream_l2[0]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in12, in_stream_l2[1]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in13, in_stream_l2[2]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in14, in_stream_l2[3]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in15, in_stream_l2[4]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in16, in_stream_l2[5]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in17, in_stream_l2[6]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in18, in_stream_l2[7]);

        switch_16x16_frp< FRP_TEMPLATE(default_route) >(
            in_stream_l1[0], in_stream_l1[1], in_stream_l1[2], in_stream_l1[3], in_stream_l1[4], in_stream_l1[5], in_stream_l1[6], in_stream_l1[7],
            in_stream_l2[0], in_stream_l2[1], in_stream_l2[2], in_stream_l2[3], in_stream_l2[4], in_stream_l2[5], in_stream_l2[6], in_stream_l2[7],
            out_stream_l1[0], out_stream_l1[1], out_stream_l1[2], out_stream_l1[3], out_stream_l1[4], out_stream_l1[5], out_stream_l1[6], out_stream_l1[7],
            out_stream_l2[0], out_stream_l2[1], out_stream_l2[2], out_stream_l2[3], out_stream_l2[4], out_stream_l2[5], out_stream_l2[6], out_stream_l2[7]
        );

        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l1[0], out01);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l1[1], out02);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l1[2], out03);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l1[3], out04);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l1[4], out05);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l1[5], out06);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l1[6], out07);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l1[7], out08);

        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l2[0], out11);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l2[1], out12);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l2[2], out13);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l2[3], out14);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l2[4], out15);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l2[5], out16);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l2[6], out17);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream_l2[7], out18);


    }
}

