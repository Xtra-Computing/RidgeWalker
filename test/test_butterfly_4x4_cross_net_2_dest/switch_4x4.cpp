#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>

#define NUM_OF_PAYLOAD_PER_FRAME    (4)
#define NETWORK_FRAME_WIDTH         (512)

#define CURRENT_NODE_ID             (0x0)
#define LOCAL_NODE_ID_BITS          (2)
#define NUM_OF_SWITCH_STREAMS       (1 << LOCAL_NODE_ID_BITS)

//debug_msg_inner_stream_t (&in)[8]

#define TEST_DEST  (1)


void remote_filter(default_route_frp_inner_stream_t     (&in_stream)[4],
                   default_route_frp_inner_stream_t     (&local_stream)[4],
                   net_stream_t                         &rout,
                   const uint16_t                       filtered_id,
                   const uint16_t                       nbits_of_local,
                   const uint16_t                       socket_id  )
{
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        default_route_x2_frp_inner_stream_t  to_filter_stream[NUM_OF_SWITCH_STREAMS / 2];
#pragma HLS stream variable=to_filter_stream  depth=12

        default_route_x2_frp_inner_stream_t  filtered_x2_stream[NUM_OF_SWITCH_STREAMS / 2];
#pragma HLS stream variable=filtered_x2_stream  depth=12

        default_route_x4_frp_inner_stream_t  remote_out;
#pragma HLS stream variable=remote_out  depth=12

        default_route_x4_frp_inner_stream_t  remote_filter_out;
#pragma HLS stream variable=remote_filter_out  depth=12

        default_route_frp_inner_stream_t     to_coalesce_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=to_coalesce_stream  depth=12


        for (int i  = 0; i < 4 ; i++)
        {
#pragma HLS unroll
                filter_to_other_node< FRP_TEMPLATE(default_route) >(in_stream[i], local_stream[i],
                                to_coalesce_stream[i], filtered_id,  nbits_of_local);
        }
        coalesce_with_bubble<default_route_frp_item_t, default_route_x2_frp_item_t>(to_coalesce_stream[0], to_coalesce_stream[1],
                        to_filter_stream[0], sizeof(default_route_t) * 8);
        coalesce_with_bubble<default_route_frp_item_t, default_route_x2_frp_item_t>(to_coalesce_stream[2], to_coalesce_stream[3],
                        to_filter_stream[1], sizeof(default_route_t) * 8);

        filter_bubble_in_pair< default_route_x2_frp_item_t >(
                to_filter_stream[0], filtered_x2_stream[0], sizeof(default_route_t) * 8, 16);

        filter_bubble_in_pair< default_route_x2_frp_item_t >(
                to_filter_stream[1], filtered_x2_stream[1], sizeof(default_route_t) * 8, 16);

        coalesce_with_bubble<default_route_x2_frp_item_t, default_route_x4_frp_item_t>(filtered_x2_stream[0], filtered_x2_stream[1],
                        remote_filter_out, sizeof(default_route_x2_t) * 8);

        filter_bubble_in_pair< default_route_x4_frp_item_t >(
                remote_filter_out, remote_out, sizeof(default_route_x2_t) * 8, 16);

        free_run_frp_to_net_timeout< FRP_TEMPLATE(default_route_x4) >(remote_out, rout, socket_id, 17, 16);
}

extern "C" {
        void switch_4x4(
                default_route_stream_t   &in1,
                default_route_stream_t   &in2,
                default_route_stream_t   &in3,
                default_route_stream_t   &in4,
                default_route_stream_t   &out1,
                default_route_stream_t   &out2,
                default_route_stream_t   &out3,
                default_route_stream_t   &out4,
                net_stream_t             &rin,
                net_stream_t             &rout
        )
        {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

                default_route_frp_inner_stream_t     in_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=in_stream  depth=12

                default_route_frp_inner_stream_t     rin_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=rin_stream  depth=12



                net_frp_inner_stream_t  remote_in;
#pragma HLS stream variable=remote_in  depth=12




                default_route_frp_inner_stream_t     local_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=local_stream  depth=12


                default_route_frp_inner_stream_t     local_merged_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=local_merged_stream  depth=12

                default_route_frp_inner_stream_t     out_stream[NUM_OF_SWITCH_STREAMS];
#pragma HLS stream variable=out_stream  depth=12


                free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in1, in_stream[0]);
                free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in2, in_stream[1]);
                free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in3, in_stream[2]);
                free_run_axis_to_frp_stream< FRP_TEMPLATE(default_route) >(in4, in_stream[3]);
                free_run_axis_to_frp_stream< FRP_TEMPLATE(net) >(rin, remote_in);


                dispatch_remote_package< net_frp_item_t, default_route_frp_item_t, default_route_x2_item_t >(
                        remote_in, rin_stream, sizeof(default_route_t) * 8);

                remote_filter(in_stream, local_stream, rout,  TEST_DEST,  LOCAL_NODE_ID_BITS, 1);



                for (int i  = 0; i < 4 ; i++)
                {
#pragma HLS unroll
                        merge_package< FRP_TEMPLATE(default_route) >(local_stream[i], rin_stream[i],
                                        local_merged_stream[i]);
                }



                switch_4x4_frp< FRP_TEMPLATE(default_route) >(
                        local_merged_stream[0], local_merged_stream[1], local_merged_stream[2], local_merged_stream[3],
                        out_stream[0], out_stream[1], out_stream[2], out_stream[3]
                );

                free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[0], out1);
                free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[1], out2);
                free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[2], out3);
                free_run_frp_to_axis_stream< FRP_TEMPLATE(default_route) >(out_stream[3], out4);



        }
}






//   coalesce_with_bubble<default_route_frp_item_t,   timeout_debug_frp_item_t>(
//           in_stream[0], in_stream[1], filter_bubble_stream, sizeof(default_route_t) * 8);

//        filter_bubble_in_pair< timeout_debug_frp_item_t >(
//            filter_bubble_stream, out_stream, sizeof(default_route_t) * 8, 16);
