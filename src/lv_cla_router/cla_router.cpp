#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"
#include "cla_switch_type.h"

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void cla_input (    cla_task_stream_t      &input,
                    frp_stream_t            &frp )
{
#pragma HLS interface ap_ctrl_none port=return

    while (true)
    {
        cla_task_pkg_t   cla_pkg;
        frp_t  route_vertex_frp;

        if (input.read_nb(cla_pkg))
        {

            lcolumn_access_item_t v = cla_pkg.data.ap_cla_ca();
            frp_t  rv;
            rv.ap_member(struct_t, dest) = v.ap_cl_dest() >> 1;
            rv.ap_member(struct_t, data) = cla_pkg.data;

            route_vertex_frp.ap_frp_data(struct_t) = rv;
            route_vertex_frp.ap_frp_last(struct_t) = cla_pkg.last;
            frp.write(route_vertex_frp);
        }
    }
}

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void cla_output (   frp_stream_t            &in_frp,
                    cla_task_stream_t      &output)
{
#pragma HLS interface ap_ctrl_none port=return

    while (true)
    {
        cla_task_pkg_t cla_pkg;
        frp_t           route_vertex_frp;

        if (in_frp.read_nb(route_vertex_frp))
        {

            frp_t  rv = route_vertex_frp.ap_frp_data(struct_t);

            cla_pkg.data = rv.ap_member(struct_t, data);
            cla_pkg.last = route_vertex_frp.ap_frp_last(struct_t);
            output.write(cla_pkg);
        }
    }
}



extern "C" {
    void cla_router(
        cla_task_stream_t    &in1,
        cla_task_stream_t    &in2,
        cla_task_stream_t    &in3,
        cla_task_stream_t    &in4,
        cla_task_stream_t    &in5,
        cla_task_stream_t    &in6,
        cla_task_stream_t    &in7,
        cla_task_stream_t    &in8,

        cla_task_stream_t    &out1,
        cla_task_stream_t    &out2,
        cla_task_stream_t    &out3,
        cla_task_stream_t    &out4,
        cla_task_stream_t    &out5,
        cla_task_stream_t    &out6,
        cla_task_stream_t    &out7,
        cla_task_stream_t    &out8

    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        route_cla_frp_inner_stream_t     in_stream[8];
#pragma HLS stream variable=in_stream  depth=13

        route_cla_frp_inner_stream_t     out_stream[8];
#pragma HLS stream variable=out_stream  depth=13

        cla_input< FRP_TEMPLATE(route_cla) >(in1, in_stream[0]);
        cla_input< FRP_TEMPLATE(route_cla) >(in2, in_stream[1]);
        cla_input< FRP_TEMPLATE(route_cla) >(in3, in_stream[2]);
        cla_input< FRP_TEMPLATE(route_cla) >(in4, in_stream[3]);
        cla_input< FRP_TEMPLATE(route_cla) >(in5, in_stream[4]);
        cla_input< FRP_TEMPLATE(route_cla) >(in6, in_stream[5]);
        cla_input< FRP_TEMPLATE(route_cla) >(in7, in_stream[6]);
        cla_input< FRP_TEMPLATE(route_cla) >(in8, in_stream[7]);


        switch_8x8_frp< FRP_TEMPLATE(route_cla) >(
            in_stream[0], in_stream[1], in_stream[2], in_stream[3], in_stream[4], in_stream[5], in_stream[6], in_stream[7],
            out_stream[0], out_stream[1], out_stream[2], out_stream[3], out_stream[4], out_stream[5], out_stream[6], out_stream[7]
        );

        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[0], out1);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[1], out2);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[2], out3);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[3], out4);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[4], out5);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[5], out6);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[6], out7);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[7], out8);


    }
}

