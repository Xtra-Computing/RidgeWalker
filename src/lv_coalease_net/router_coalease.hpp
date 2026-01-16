#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"

#include "stream_operation.h"


typedef struct __attribute__((packed)) {
    route_id_t dest;
    rpa_task_t data;
}  route_vertex_t;


typedef struct __attribute__((packed)) {
    uint8_t mask;
    rpa_task_t data[4];
}  route_net_vertex_t;


GEN_STRUCT_INTERFACE(route_vertex)

GEN_STRUCT_INTERFACE(route_net_vertex)

// bit  7  6  5  4  3  2  1  0
//         [ FID ]  [ HID ] BID



void rpa_to_net_with_timeout(   rpa_task_stream_t               &in,
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
        rpa_task_pkg_t in_pkg;
        if (in.read_nb(in_pkg)) {
            wait_counter = 0;
            buffer.ap_member_array(route_net_vertex_t, data, buffer_counter) = in_pkg.data;

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
