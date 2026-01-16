#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"

#include "stream_operation.h"


#include "router_coalease.hpp"




extern "C" {
    void router_coalease_c(

        rpa_task_stream_t    &out,

        net_stream_t         &rin

    ){
        route_vertex_frp_inner_stream_t     remote_in_stream[8];
#pragma HLS stream variable=remote_in_stream  depth=13


        route_vertex_frp_inner_stream_t     l1_merge[4];
#pragma HLS stream variable=l1_merge  depth=13

        route_vertex_frp_inner_stream_t     l2_merge[2];
#pragma HLS stream variable=l2_merge  depth=13

        route_vertex_frp_inner_stream_t     l3_merge;
#pragma HLS stream variable=l3_merge  depth=13


#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        dispatch_net_package(rin, remote_in_stream);


        merge_package< FRP_TEMPLATE(route_vertex) >(remote_in_stream[0], remote_in_stream[1], l1_merge[0]);
        merge_package< FRP_TEMPLATE(route_vertex) >(remote_in_stream[2], remote_in_stream[3], l1_merge[1]);
        merge_package< FRP_TEMPLATE(route_vertex) >(remote_in_stream[4], remote_in_stream[5], l1_merge[2]);
        merge_package< FRP_TEMPLATE(route_vertex) >(remote_in_stream[6], remote_in_stream[7], l1_merge[3]);

        merge_package< FRP_TEMPLATE(route_vertex) >(l1_merge[0], l1_merge[1], l2_merge[0]);
        merge_package< FRP_TEMPLATE(route_vertex) >(l1_merge[2], l1_merge[3], l2_merge[1]);

        merge_package< FRP_TEMPLATE(route_vertex) >(l2_merge[0], l2_merge[1], l3_merge);

        rpa_output< FRP_TEMPLATE(route_vertex) >(l3_merge, out);


    }
}


