#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>





extern "C" {
    void timeout_merge(
        default_route_stream_t   &in1,
        default_route_stream_t   &in2,
        timeout_debug_stream_t   &out
    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        default_route_frp_inner_stream_t     in_stream[2];
#pragma HLS stream variable=in_stream  depth=4


        timeout_debug_frp_inner_stream_t     out_stream;
#pragma HLS stream variable=out_stream  depth=4

        timeout_debug_frp_inner_stream_t     filter_bubble_stream;
#pragma HLS stream variable=filter_bubble_stream  depth=4


        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in1, in_stream[0]);
        free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in2, in_stream[1]);

        coalesce_with_bubble<default_route_frp_item_t,   timeout_debug_frp_item_t>(
           in_stream[0], in_stream[1], filter_bubble_stream, sizeof(default_route_t) * 8);

        filter_bubble_in_pair< timeout_debug_frp_item_t >(
            filter_bubble_stream, out_stream, sizeof(default_route_t) * 8, 16);

        free_run_frp_to_axis_stream< FRP_TEMPLATE(timeout_debug) >(out_stream, out);
    }
}




