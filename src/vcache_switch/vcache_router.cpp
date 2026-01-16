#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"
#include "vcache_switch_type.h"

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void next_vertex_router (   next_vertex_stream_t                &input,
                            frp_stream_t                 &frp )
{
#pragma HLS interface ap_ctrl_none port=return

    while (true)
    {
        next_vertex_pkg_t nv_pkg;
        frp_t  route_vertex_frp;

        if (input.read_nb(nv_pkg))
        {
            uint64_t v = nv_pkg.data.ap_member(next_vertex_t, vertex);
            frp_t  rv;
            rv.ap_member(struct_t, dest) = v & 0x7;
            rv.ap_member(struct_t, data) = v >> 3;
            route_vertex_frp.ap_frp_data(struct_t) = rv;
            route_vertex_frp.ap_frp_last(struct_t) = nv_pkg.last;
            frp.write(route_vertex_frp);
        }
    }
}

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void next_vertex_output (   frp_stream_t      &in_frp,
                            next_vertex_stream_t                 &output)
{
#pragma HLS interface ap_ctrl_none port=return

    while (true)
    {
        next_vertex_pkg_t nv_pkg;
        frp_t  route_vertex_frp;

        if (in_frp.read_nb(route_vertex_frp))
        {
            uint64_t v = nv_pkg.data.ap_member(next_vertex_t, vertex);

            frp_t  rv = route_vertex_frp.ap_frp_data(struct_t);

            nv_pkg.data.ap_member(next_vertex_t, vertex) =  rv.ap_member(struct_t, data);
            nv_pkg.last =  route_vertex_frp.ap_frp_last(struct_t);
            output.write(nv_pkg);
        }
    }
}



extern "C" {
    void vcache_router(
        next_vertex_stream_t    &in1,
        next_vertex_stream_t    &in2,
        next_vertex_stream_t    &in3,
        next_vertex_stream_t    &in4,
        next_vertex_stream_t    &in5,
        next_vertex_stream_t    &in6,
        next_vertex_stream_t    &in7,
        next_vertex_stream_t    &in8,

        next_vertex_stream_t    &out1,
        next_vertex_stream_t    &out2,
        next_vertex_stream_t    &out3,
        next_vertex_stream_t    &out4,
        next_vertex_stream_t    &out5,
        next_vertex_stream_t    &out6,
        next_vertex_stream_t    &out7,
        next_vertex_stream_t    &out8

    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        route_vertex_frp_inner_stream_t     in_stream[8];
#pragma HLS stream variable=in_stream  depth=13

        route_vertex_frp_inner_stream_t     out_stream[8];
#pragma HLS stream variable=out_stream  depth=13

        next_vertex_router< FRP_TEMPLATE(route_vertex) >(in1, in_stream[0]);
        next_vertex_router< FRP_TEMPLATE(route_vertex) >(in2, in_stream[1]);
        next_vertex_router< FRP_TEMPLATE(route_vertex) >(in3, in_stream[2]);
        next_vertex_router< FRP_TEMPLATE(route_vertex) >(in4, in_stream[3]);
        next_vertex_router< FRP_TEMPLATE(route_vertex) >(in5, in_stream[4]);
        next_vertex_router< FRP_TEMPLATE(route_vertex) >(in6, in_stream[5]);
        next_vertex_router< FRP_TEMPLATE(route_vertex) >(in7, in_stream[6]);
        next_vertex_router< FRP_TEMPLATE(route_vertex) >(in8, in_stream[7]);


        switch_8x8_frp< FRP_TEMPLATE(route_vertex) >(
            in_stream[0], in_stream[1], in_stream[2], in_stream[3], in_stream[4], in_stream[5], in_stream[6], in_stream[7],
            out_stream[0], out_stream[1], out_stream[2], out_stream[3], out_stream[4], out_stream[5], out_stream[6], out_stream[7]
        );

        next_vertex_output< FRP_TEMPLATE(route_vertex) >(out_stream[0], out1);
        next_vertex_output< FRP_TEMPLATE(route_vertex) >(out_stream[1], out2);
        next_vertex_output< FRP_TEMPLATE(route_vertex) >(out_stream[2], out3);
        next_vertex_output< FRP_TEMPLATE(route_vertex) >(out_stream[3], out4);
        next_vertex_output< FRP_TEMPLATE(route_vertex) >(out_stream[4], out5);
        next_vertex_output< FRP_TEMPLATE(route_vertex) >(out_stream[5], out6);
        next_vertex_output< FRP_TEMPLATE(route_vertex) >(out_stream[6], out7);
        next_vertex_output< FRP_TEMPLATE(route_vertex) >(out_stream[7], out8);


    }
}

