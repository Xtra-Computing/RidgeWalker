#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>



extern "C" {
    void switch_2x2(
        default_route_stream_t   &in1,
        default_route_stream_t   &in2,
        default_route_stream_t   &out1,
        default_route_stream_t   &out2
    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        default_route_frp_inner_stream_t     in_stream[2];
#pragma HLS stream variable=in_stream  depth=2


        default_route_frp_inner_stream_t     out_stream[2];
#pragma HLS stream variable=out_stream  depth=2

        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in1, in_stream[0]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in2, in_stream[1]);

        switch_2x2_frp< FRP_TEMPLATE(default_route) >(  in_stream[0],in_stream[1],
                                                        out_stream[0],out_stream[1],
                                                        0, 1, 0x1);

        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[0], out1);
        free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[1], out2);

    }
}

