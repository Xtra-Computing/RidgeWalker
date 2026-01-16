#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"
#include "rpa_switch_type.h"

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>

void rpa_input (    rpa_task_stream_t      &input,
                    frp_stream_t            &frp )
{
#pragma HLS interface ap_ctrl_none port=return

    while (true)
    {
        rpa_task_pkg_t   rpa_pkg;
        frp_t  route_vertex_frp;

        if (input.read_nb(rpa_pkg))
        {

            uint64_t v = rpa_pkg.data.ap_rpa_vid();
            frp_t  rv;
            rv.ap_member(struct_t, dest) = (v & 0xf)>>1;
            rv.ap_member(struct_t, data) = rpa_pkg.data;

            route_vertex_frp.ap_frp_data(struct_t) = rv;
            route_vertex_frp.ap_frp_last(struct_t) = rpa_pkg.last;
            frp.write(route_vertex_frp);
        }
    }
}

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void rpa_output (   frp_stream_t            &in_frp,
                    rpa_task_stream_t      &output)
{
#pragma HLS interface ap_ctrl_none port=return

    while (true)
    {
        rpa_task_pkg_t rpa_pkg;
        frp_t           route_vertex_frp;

        if (in_frp.read_nb(route_vertex_frp))
        {

            frp_t  rv = route_vertex_frp.ap_frp_data(struct_t);

            rpa_pkg.data = rv.ap_member(struct_t, data);
            rpa_pkg.last = route_vertex_frp.ap_frp_last(struct_t);
            output.write(rpa_pkg);
        }
    }
}



extern "C" {
    void rpa_router(
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

        route_vertex_frp_inner_stream_t     in_stream[8];
#pragma HLS stream variable=in_stream  depth=13

        route_vertex_frp_inner_stream_t     out_stream[8];
#pragma HLS stream variable=out_stream  depth=13

        rpa_input< FRP_TEMPLATE(route_vertex) >(in1, in_stream[0]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in2, in_stream[1]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in3, in_stream[2]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in4, in_stream[3]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in5, in_stream[4]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in6, in_stream[5]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in7, in_stream[6]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in8, in_stream[7]);


        switch_8x8_frp< FRP_TEMPLATE(route_vertex) >(
            in_stream[0], in_stream[1], in_stream[2], in_stream[3], in_stream[4], in_stream[5], in_stream[6], in_stream[7],
            out_stream[0], out_stream[1], out_stream[2], out_stream[3], out_stream[4], out_stream[5], out_stream[6], out_stream[7]
        );

        rpa_output< FRP_TEMPLATE(route_vertex) >(out_stream[0], out1);
        rpa_output< FRP_TEMPLATE(route_vertex) >(out_stream[1], out2);
        rpa_output< FRP_TEMPLATE(route_vertex) >(out_stream[2], out3);
        rpa_output< FRP_TEMPLATE(route_vertex) >(out_stream[3], out4);
        rpa_output< FRP_TEMPLATE(route_vertex) >(out_stream[4], out5);
        rpa_output< FRP_TEMPLATE(route_vertex) >(out_stream[5], out6);
        rpa_output< FRP_TEMPLATE(route_vertex) >(out_stream[6], out7);
        rpa_output< FRP_TEMPLATE(route_vertex) >(out_stream[7], out8);


    }
}

