#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rpa_switch_type.h"

#include "butterfly_network.hpp"

#include "stream_operation.h"




// bit  7  6  5  4  3  2  1  0
//         [ FID ]  [ HID ] BID



void rpa_to_net_with_timeout(   route_vertex_frp_inner_stream_t &in,
                                net_frame_frp_inner_stream_t    &net,
                                const int dest,
                                const int frame_size,
                                const int timeout
                            )
{
#pragma HLS interface ap_ctrl_none port=return

    route_net_vertex_item_t buffer = 0;
    ap_uint<2> buffer_counter = 0;
    ap_uint<8> wait_counter = 0;

    ap_uint<8> frame_counter = 0;


coalease: while (true)
    {
        route_vertex_frp_item_t in_pkg;
        if (in.read_nb(in_pkg)) {
            wait_counter = 0;
            rpa_task_item_t rpa_task = in_pkg.ap_member(route_vertex_t, data);
            buffer.ap_member_array(route_net_vertex_t, data, buffer_counter) = rpa_task;

            if (buffer_counter == 3) {
                buffer.ap_member(route_net_vertex_t, mask) = 0xf;

                net_frame_frp_item_t frp_package;

                net_frame_item_t net_item = 0;
                net_item.ap_member(net_frame_t, data) = buffer;
                net_item.ap_member(net_frame_t, dest) = dest;

                if (frame_counter >= frame_size) {
                    frp_package.ap_frp_last(net_frame_t) = 1;
                    frame_counter = 0;
                }
                else {
                    frp_package.ap_frp_last(net_frame_t) = 0;
                    frame_counter ++;
                }

                frp_package.ap_frp_data(net_frame_t) = net_item;
                net.write(frp_package);


                buffer_counter = 0;
            }
            else {
                buffer_counter ++;
            }

        } else {
            if (buffer_counter != 0) {
                wait_counter ++;
                if (wait_counter > timeout) {

                    uint8_t mask;
                    if (buffer_counter == 1)
                        mask = 0x1;
                    else if (buffer_counter == 2)
                        mask = 0x3;
                    else
                        mask = 0x7;

                    buffer.ap_member(route_net_vertex_t, mask) = mask;

                    net_frame_frp_item_t frp_package;
                    net_frame_item_t net_item = 0;

                    net_item.ap_member(net_frame_t, data) = buffer;
                    net_item.ap_member(net_frame_t, dest) = dest;
                    frp_package.ap_frp_last(net_frame_t) = 1;
                    frp_package.ap_frp_data(net_frame_t) = net_item;

                    net.write(frp_package);

                    buffer_counter = 0;

                    frame_counter = 0;
                }
            }
        }
    }
}



void dispatch_net_package(  net_stream_t                        &rin,
                            route_vertex_frp_inner_stream_t     (&out)[8]
                         )
{

#pragma HLS interface ap_ctrl_none port=return


    ap_uint<1> schedule = 0;
remote: while (true)    {
#pragma HLS pipeline  II=1

        net_pkg_t net_pkg;
        if (rin.read_nb(net_pkg)) {
            net_item_t net_data = net_pkg.data;

            uint8_t mask = net_data.ap_member(route_net_vertex_t, mask);
            if (schedule == 0) {

                for (int i = 0; i < 4; i++) {
#pragma HLS unroll
                    if (mask & (1 << i))
                    {
                        rpa_task_item_t rpa = net_data.ap_member_array(route_net_vertex_t, data, i);
                        uint64_t v = rpa.ap_rpa_vid();

                        route_vertex_frp_item_t  rv;

                        rv.ap_member(route_vertex_t, dest) = (v & 0xf) >> 1;
                        rv.ap_member(route_vertex_t, data) = rpa;
                        rv.ap_frp_last(route_vertex_t) = 0;

                        out[i].write(rv);
                    }
                }
                schedule = 1;
            }
            else {
                for (int i = 0; i < 4; i++) {
#pragma HLS unroll
                    if (mask & (1 << i))
                    {
                        rpa_task_item_t rpa = net_data.ap_member_array(route_net_vertex_t, data, i);
                        uint64_t v = rpa.ap_rpa_vid();

                        route_vertex_frp_item_t  rv;

                        rv.ap_member(route_vertex_t, dest) = (v & 0xf) >> 1;
                        rv.ap_member(route_vertex_t, data) = rpa;
                        rv.ap_frp_last(route_vertex_t) = 0;

                        out[i + 4].write(rv);
                    }
                }
                schedule = 0;
            }
        }
    }
}







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
            rv.ap_member(struct_t, dest) = (v & 0xf) >> 1;
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





template < typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t >
void router_mapper_instance(frp_stream_t &stream_in,
                            frp_stream_t &local_out,
                            frp_stream_t &remote_out,
                            router_mapper_frp_inner_stream_t &map_in,
                            router_mapper_frp_inner_stream_t &map_out )
{

#pragma HLS interface ap_ctrl_none port=return

    ap_uint<3> map_array[8];
#pragma HLS ARRAY_PARTITION variable=map_array   dim=0
#pragma HLS DEPENDENCE variable=map_array type=inter false
#pragma HLS DEPENDENCE variable=map_array type=inter false



    while (1) {
        router_mapper_frp_item_t map_data;
        if (map_in.read_nb(map_data))
        {
            ap_uint<3> index = map_data.ap_member(router_mapper_t, ori_id);
            map_array[index] = map_data.ap_member(router_mapper_t, mapped_id);
            map_out.write(map_data);
        }

        frp_t frp_item;
        if (stream_in.read_nb(frp_item))
        {

            //
            rpa_task_item_t rpa = frp_item.ap_member(struct_t, data);
            ap_uint<40> v = rpa.ap_rpa_vid();
            //

            ap_uint<3> old_id = v >> 29; // 4GB -> 512M vertex
            ap_uint<3> new_id = map_array[old_id];

            if (new_id == 0)
            {
                frp_t  rv;
                ap_uint<8> new_dest;
                ap_uint<8> old_dest  = frp_item.ap_member(struct_t, dest);
                new_dest.range(3, 0) = old_dest.range(3, 0);
                new_dest.range(7, 4) = 0;
                rv.ap_member(struct_t, dest) = new_dest;
                rv.ap_member(struct_t, data) = frp_item.ap_member(struct_t, data);
                rv.ap_frp_last(struct_t) = 0;
                local_out.write(rv);
            }
            else
            {
                frp_t  rv;
                ap_uint<8> new_dest;
                new_dest.range(2, 0) = new_id;  //remote router use fpga id
                new_dest.range(7, 3) = 0;
                rv.ap_member(struct_t, dest) = new_dest;
                rv.ap_member(struct_t, data) = frp_item.ap_member(struct_t, data);
                rv.ap_frp_last(struct_t) = 0;
                remote_out.write(rv);
            }

        }

    }
}




extern "C" {
    void rpa_crouter(
        router_mapper_stream_t  & map,

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
        rpa_task_stream_t    &out8,

        net_stream_t         &rin,
        net_stream_t         &rout
    )
    {


#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow


        router_mapper_frp_inner_stream_t     map_stream[9];
#pragma HLS stream variable=map_stream  depth=13


        route_vertex_frp_inner_stream_t     in_stream[8];
#pragma HLS stream variable=in_stream  depth=13


        route_vertex_frp_inner_stream_t     local_stream[8];
#pragma HLS stream variable=local_stream  depth=13

        route_vertex_frp_inner_stream_t     remote_stream[8];
#pragma HLS stream variable=remote_stream  depth=26


        route_vertex_frp_inner_stream_t     remote_in_stream[8];
#pragma HLS stream variable=remote_in_stream  depth=13


        route_vertex_frp_inner_stream_t     merged_stream[8];
#pragma HLS stream variable=merged_stream  depth=13


        route_vertex_frp_inner_stream_t     out_stream[8];
#pragma HLS stream variable=out_stream  depth=13


        route_vertex_frp_inner_stream_t     rout_stream[8];
#pragma HLS stream variable=rout_stream  depth=13





#define __N_STREAM__ (8)

        net_frame_frp_inner_stream_t  coaleased_frame[__N_STREAM__];
#pragma HLS stream variable=coaleased_frame  depth=13

        net_frame_frp_inner_stream_t  coaleased_frame_framing[__N_STREAM__];
#pragma HLS stream variable=coaleased_frame_framing  depth=510

        net_signal_inner_stream_t  frame_signal[__N_STREAM__];
#pragma HLS stream variable=frame_signal  depth=510


        net_frame_frp_inner_stream_t l1_switch[__N_STREAM__];
#pragma HLS stream variable=l1_switch  depth=31

        net_frame_frp_inner_stream_t l2_switch[__N_STREAM__ / 2];
#pragma HLS stream variable=l1_switch  depth=31

        net_frame_frp_inner_stream_t l3_switch[__N_STREAM__ / 4];
#pragma HLS stream variable=l1_switch  depth=31

        net_frame_frp_inner_stream_t l4_switch;
#pragma HLS stream variable=l2_switch  depth=31



        free_run_axis_to_frp_stream< FRP_TEMPLATE(router_mapper) >(map, map_stream[0]);


        rpa_input< FRP_TEMPLATE(route_vertex) >(in1, in_stream[0]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in2, in_stream[1]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in3, in_stream[2]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in4, in_stream[3]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in5, in_stream[4]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in6, in_stream[5]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in7, in_stream[6]);
        rpa_input< FRP_TEMPLATE(route_vertex) >(in8, in_stream[7]);


        for (int i = 0; i < 8; i ++)
        {
#pragma HLS unroll
            router_mapper_instance< FRP_TEMPLATE(route_vertex) >(
                in_stream[i],
                local_stream[i], remote_stream[i],
                map_stream[i], map_stream[i + 1]);
        }

        frp_sink_stream< FRP_TEMPLATE(router_mapper) >(map_stream[8]);

        dispatch_net_package(rin, remote_in_stream);


        for (int i  = 0; i < 8 ; i++)
        {
#pragma HLS unroll
            merge_package< FRP_TEMPLATE(route_vertex) >(local_stream[i], remote_in_stream[i],
                    merged_stream[i]);
        }


        switch_8x8_frp< FRP_TEMPLATE(route_vertex) >(
            merged_stream[0], merged_stream[1], merged_stream[2], merged_stream[3], merged_stream[4], merged_stream[5], merged_stream[6], merged_stream[7],
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

        switch_8x8_frp< FRP_TEMPLATE(route_vertex) >(
            remote_stream[0], remote_stream[1], remote_stream[2], remote_stream[3], remote_stream[4], remote_stream[5], remote_stream[6], remote_stream[7],
            rout_stream[0], rout_stream[1], rout_stream[2], rout_stream[3], rout_stream[4], rout_stream[5], rout_stream[6], rout_stream[7]
        );

        //rpa_output< FRP_TEMPLATE(route_vertex) >(rout_stream[0], rout1);
        //rpa_output< FRP_TEMPLATE(route_vertex) >(rout_stream[1], rout2);
        //rpa_output< FRP_TEMPLATE(route_vertex) >(rout_stream[2], rout3);
        //rpa_output< FRP_TEMPLATE(route_vertex) >(rout_stream[3], rout4);
        //rpa_output< FRP_TEMPLATE(route_vertex) >(rout_stream[4], rout5);
        //rpa_output< FRP_TEMPLATE(route_vertex) >(rout_stream[5], rout6);
        //rpa_output< FRP_TEMPLATE(route_vertex) >(rout_stream[6], rout7);
        //rpa_output< FRP_TEMPLATE(route_vertex) >(rout_stream[7], rout8);

#define __TIMEOUT__  (22)
#define __NET_FRAME_SIZE__  (20)

        for (int i = 0; i < __N_STREAM__; i++) {
#pragma HLS unroll
            rpa_to_net_with_timeout(rout_stream[i], coaleased_frame[i], ((const int )(i + 1)), __NET_FRAME_SIZE__, __TIMEOUT__);
        }

        for (int i = 0; i < __N_STREAM__; i++) {
#pragma HLS unroll
            net_framing_signal_generator(coaleased_frame[i], coaleased_frame_framing[i], frame_signal[i], __TIMEOUT__);
        }

        for (int i = 0; i < __N_STREAM__; i++) {
#pragma HLS unroll
            net_framing(coaleased_frame_framing[i], l1_switch[i], frame_signal[i]);
        }
        // hardcoded

        for (int i = 0; i < __N_STREAM__ / 2; i++) {
#pragma HLS unroll
            net_scheduler(l1_switch[0 + 2 * i], l1_switch[1 + 2 * i], l2_switch[i]);
        }

        for (int i = 0; i < __N_STREAM__ / 4; i++) {
#pragma HLS unroll
            net_scheduler(l2_switch[0 + 2 * i], l2_switch[1 + 2 * i], l3_switch[i]);
        }

        net_scheduler(l3_switch[0], l3_switch[1], l4_switch);

        net_frame_out(l4_switch, rout);

    }
}

