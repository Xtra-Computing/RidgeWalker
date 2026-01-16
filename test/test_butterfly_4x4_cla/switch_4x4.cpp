#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>

#include "rw_type.h"

#define NUM_OF_PAYLOAD_PER_FRAME    (4)
#define NETWORK_FRAME_WIDTH         (512)

#define CURRENT_NODE_ID             (0x0)
#define LOCAL_NODE_ID_BITS          (2)
#define NUM_OF_SWITCH_STREAMS       (1 << LOCAL_NODE_ID_BITS)

//debug_msg_inner_stream_t (&in)[8]

typedef struct __attribute__((packed)){
    route_id_t dest;
    cla_task_t data;
}  route_cla_t;

typedef struct __attribute__((packed)){
    route_cla_t data[2];
    uint8_t reserved;
}  route_cla_x2_t;


typedef struct __attribute__((packed)){
    route_cla_x2_t data[2];
    uint8_t reserved;
}  route_cla_x4_t;


GEN_STRUCT_INTERFACE(route_cla)
GEN_STRUCT_INTERFACE(route_cla_x2)
GEN_STRUCT_INTERFACE(route_cla_x4)


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
            rv.ap_member(struct_t, dest) = v.ap_cl_dest();
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
    void switch_4x4(
        cla_task_stream_t   &in1,
        cla_task_stream_t   &in2,
        cla_task_stream_t   &in3,
        cla_task_stream_t   &in4,
        cla_task_stream_t   &out1,
        cla_task_stream_t   &out2,
        cla_task_stream_t   &out3,
        cla_task_stream_t   &out4,
        //net_stream_t        &rin,
        //net_stream_t        &rout
        route_cla_x4_stream_t &rin,
        route_cla_x4_stream_t &rout
    )
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        route_cla_frp_inner_stream_t     in_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=in_stream  depth=12

        route_cla_frp_inner_stream_t     rin_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=rin_stream  depth=12

        route_cla_frp_inner_stream_t     to_coalesce_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=to_coalesce_stream  depth=12


        route_cla_x2_frp_inner_stream_t  to_filter_stream[NUM_OF_SWITCH_STREAMS / 2];
#pragma HLS stream variable=to_filter_stream  depth=12


        route_cla_x2_frp_inner_stream_t  filtered_x2_stream[NUM_OF_SWITCH_STREAMS / 2];
#pragma HLS stream variable=filtered_x2_stream  depth=12

        route_cla_x4_frp_inner_stream_t  remote_in;
#pragma HLS stream variable=remote_in  depth=12

        route_cla_x4_frp_inner_stream_t  remote_out;
#pragma HLS stream variable=remote_out  depth=12

        route_cla_x4_frp_inner_stream_t  remote_filter_out;
#pragma HLS stream variable=remote_filter_out  depth=12


        route_cla_frp_inner_stream_t     local_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=local_stream  depth=12


        route_cla_frp_inner_stream_t     local_merged_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=local_merged_stream  depth=12

        route_cla_frp_inner_stream_t     out_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=out_stream  depth=12


        cla_input< FRP_TEMPLATE(route_cla) >(in1, in_stream[0]);
        cla_input< FRP_TEMPLATE(route_cla) >(in2, in_stream[1]);
        cla_input< FRP_TEMPLATE(route_cla) >(in3, in_stream[2]);
        cla_input< FRP_TEMPLATE(route_cla) >(in4, in_stream[3]);

        free_run_axis_to_frp_stream< FRP_TEMPLATE(route_cla_x4) >(rin, remote_in);


        dispatch_remote_package< route_cla_x4_frp_item_t, route_cla_frp_item_t, route_cla_x2_item_t >(
                remote_in, rin_stream, sizeof(route_cla_t) * 8);


        for (int i  = 0; i < 4 ; i++)
        {
#pragma HLS unroll
            filter_to_other_node< FRP_TEMPLATE(route_cla) >(in_stream[i], local_stream[i],
                to_coalesce_stream[i], 1,  LOCAL_NODE_ID_BITS);
        }
        coalesce_with_bubble<route_cla_frp_item_t, route_cla_x2_frp_item_t>(to_coalesce_stream[0], to_coalesce_stream[1],
                to_filter_stream[0], sizeof(route_cla_t) * 8);
        coalesce_with_bubble<route_cla_frp_item_t, route_cla_x2_frp_item_t>(to_coalesce_stream[2], to_coalesce_stream[3],
                to_filter_stream[1], sizeof(route_cla_t) * 8);

        filter_bubble_in_pair< route_cla_x2_frp_item_t >(
            to_filter_stream[0], filtered_x2_stream[0], sizeof(route_cla_t) * 8, 16);

        filter_bubble_in_pair< route_cla_x2_frp_item_t >(
            to_filter_stream[1], filtered_x2_stream[1], sizeof(route_cla_t) * 8, 16);

        coalesce_with_bubble<route_cla_x2_frp_item_t, route_cla_x4_frp_item_t>(filtered_x2_stream[0], filtered_x2_stream[1],
                remote_filter_out, sizeof(route_cla_x2_t) * 8);

        filter_bubble_in_pair< route_cla_x4_frp_item_t >(
            remote_filter_out, remote_out, sizeof(route_cla_x2_t) * 8, 16);


        for (int i  = 0; i < 4 ; i++)
        {
#pragma HLS unroll
            merge_package< FRP_TEMPLATE(route_cla) >(local_stream[i], rin_stream[i],
                local_merged_stream[i]);
        }



        switch_4x4_frp< FRP_TEMPLATE(route_cla) >(
            local_merged_stream[0], local_merged_stream[1], local_merged_stream[2], local_merged_stream[3],
            out_stream[0], out_stream[1], out_stream[2], out_stream[3]
        );

        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[0], out1);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[1], out2);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[2], out3);
        cla_output< FRP_TEMPLATE(route_cla) >(out_stream[3], out4);

        free_run_frp_to_axis_stream< FRP_TEMPLATE(route_cla_x4) >(remote_out, rout);

    }
}




//   coalesce_with_bubble<route_cla_frp_item_t,   timeout_debug_frp_item_t>(
//           in_stream[0], in_stream[1], filter_bubble_stream, sizeof(route_cla_t) * 8);

//        filter_bubble_in_pair< timeout_debug_frp_item_t >(
//            filter_bubble_stream, out_stream, sizeof(route_cla_t) * 8, 16);
