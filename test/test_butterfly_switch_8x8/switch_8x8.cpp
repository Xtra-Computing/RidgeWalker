#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>



extern "C" {
    void switch_8x8(
        default_route_stream_t   &in1,
        default_route_stream_t   &in2,
        default_route_stream_t   &in3,
        default_route_stream_t   &in4,
        default_route_stream_t   &in5,
        default_route_stream_t   &in6,
        default_route_stream_t   &in7,
        default_route_stream_t   &in8,

        default_route_stream_t   &out1,
        default_route_stream_t   &out2,
        default_route_stream_t   &out3,
        default_route_stream_t   &out4,
        default_route_stream_t   &out5,
        default_route_stream_t   &out6,
        default_route_stream_t   &out7,
        default_route_stream_t   &out8

    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        default_route_frp_inner_stream_t     in_stream[8];
#pragma HLS stream variable=in_stream  depth=2

        default_route_frp_inner_stream_t     out_stream[8];
#pragma HLS stream variable=out_stream  depth=2

        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in1, in_stream[0]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in2, in_stream[1]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in3, in_stream[2]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in4, in_stream[3]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in5, in_stream[4]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in6, in_stream[5]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in7, in_stream[6]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in8, in_stream[7]);


        switch_8x8_frp< FRP_TEMPLATE(default_route) >(
            in_stream[0], in_stream[1], in_stream[2], in_stream[3], in_stream[4], in_stream[5], in_stream[6], in_stream[7],
            out_stream[0], out_stream[1], out_stream[2], out_stream[3], out_stream[4], out_stream[5], out_stream[6], out_stream[7]
            );

        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[0], out1);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[1], out2);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[2], out3);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[3], out4);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[4], out5);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[5], out6);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[6], out7);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[7], out8);


    }
}

