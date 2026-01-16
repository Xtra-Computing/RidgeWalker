#ifndef __BUTTERFLY_NETWORK_HPP__
#define __BUTTERFLY_NETWORK_HPP__


#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "stdint.h"
#include <ap_int.h>

#include "stream_operation.h"


#include "switch_type.h"

//#define SWITCH_DEBUG

const int inner_fifo_size = 12;


void net_framing_signal_generator(  net_frame_frp_inner_stream_t    &in,
                                    net_frame_frp_inner_stream_t    &out,
                                    net_signal_inner_stream_t       &signal_out,
                                    const int timeout
                                 )
{
#pragma HLS interface ap_ctrl_none port=return

    net_frame_frp_item_t last_frp_pkg;
    ap_uint<1> last_valid = 0;
    ap_uint<8> wait_counter = 0;
signaling: while (true)
    {
        net_frame_frp_item_t frp_package;
        if (in.read_nb(frp_package)) {
            wait_counter = 0;
            if (last_valid == 0) {
                last_frp_pkg = frp_package;
                last_valid = 1;
            } else {
                if (last_frp_pkg.ap_frp_last(net_frame_t) == 1) {
                    net_signal_item_t  ready = 1;
                    signal_out.write(ready);
                    out.write(last_frp_pkg);
                } else {
                    out.write(last_frp_pkg);
                }
                last_frp_pkg = frp_package;
            }
        }
        else {
            if (last_valid == 1) {
                wait_counter ++;
                if (wait_counter > (timeout + 4)) {
                    net_signal_item_t  ready = 3;
                    signal_out.write(ready);

                    net_frame_frp_item_t timeout_pkg;
                    timeout_pkg.ap_frp_last(net_frame_t) = 1;
                    timeout_pkg.ap_frp_data(net_frame_t) = last_frp_pkg.ap_frp_data(net_frame_t);
                    out.write(timeout_pkg);
                    last_valid = 0;
                }
            }
        }
    }
}


void net_framing  (net_frame_frp_inner_stream_t         &in,
                   net_frame_frp_inner_stream_t         &out,
                   net_signal_inner_stream_t            &frame_signal)
{
#pragma HLS interface ap_ctrl_none port=return

framing: while (true)
    {
#pragma HLS pipeline II = 1

        net_signal_item_t ready;
        if (frame_signal.read_nb(ready)) {
intra_framing:    for (;;) {
#pragma HLS pipeline II = 1

                net_frame_frp_item_t net_frp;
                if (in.read_nb(net_frp)) {
                    out.write(net_frp);
                    if (net_frp.ap_frp_last(net_frame_t) == 1)
                        break;
                }
            }
        }
    }
}


#define __READ_IN1__()  net_frame_frp_item_t net_frp;                     \
                        if (in1.read_nb(net_frp)) {                       \
                            out.write(net_frp);                           \
                            if (net_frp.ap_frp_last(net_frame_t) == 1) {  \
                                if (in2_empty == 1)                       \
                                    status = SCHE_INIT;                   \
                                else                                      \
                                    status = SCHE_IN2;                    \
                                schedule_flag = 0;                        \
                            }                                             \
                            else {                                        \
                                status = SCHE_IN1;                        \
                            }                                             \
                        }
#define __READ_IN2__()  net_frame_frp_item_t net_frp;                     \
                        if (in2.read_nb(net_frp)) {                       \
                            out.write(net_frp);                           \
                            if (net_frp.ap_frp_last(net_frame_t) == 1) {  \
                                if (in1_empty == 1)                       \
                                    status = SCHE_INIT;                   \
                                else                                      \
                                    status = SCHE_IN1;                    \
                                schedule_flag = 1;                        \
                            }                                             \
                            else {                                        \
                                status = SCHE_IN2;                        \
                            }                                             \
                        }

void net_scheduler( net_frame_frp_inner_stream_t         &in1,
                    net_frame_frp_inner_stream_t         &in2,
                    net_frame_frp_inner_stream_t         &out)
{
#pragma HLS interface ap_ctrl_none port=return

    enum {
        SCHE_INIT = 0, SCHE_IN1 = 1, SCHE_IN2 = 2,
    };
    volatile uint8_t status = SCHE_INIT;

    ap_uint<1> schedule_flag = 0;


scheduler: while (true)
    {
#pragma HLS pipeline II = 1
        ap_uint<1> in1_empty = in1.empty();
        ap_uint<1> in2_empty = in2.empty();
        ap_uint<3> code;
        code.range(0, 0) = schedule_flag;
        code.range(1, 1) = in1_empty;
        code.range(2, 2) = in2_empty;

        switch (status)
        {
        case SCHE_INIT: {
            switch (code)
            {   // 21S
            case 0b111:
            case 0b110:{
                status = SCHE_INIT;
                break;
            }
            case 0b101:
            case 0b100:
            case 0b001:{
               __READ_IN1__();
                break;
            }
            case 0b011:
            case 0b010:
            case 0b000:{
                __READ_IN2__();
                break;
            }
            }
            break;
        }
        case SCHE_IN1: {
            __READ_IN1__();
            break;
        }
        case SCHE_IN2: {
            __READ_IN2__();
            break;
        }
        }
    }
}


void net_frame_out( net_frame_frp_inner_stream_t     &frp,
                    net_stream_t                     &net)
{
#pragma HLS interface ap_ctrl_none port=return

frp_net: while (true)
    {
        net_frame_frp_item_t frp_item;
        if (frp.read_nb(frp_item))
        {
            net_pkg_t out_pkg;
            net_frame_item_t frame = frp_item.ap_frp_data(net_frame_t);

            out_pkg.dest = frame.ap_member(net_frame_t, dest);
            out_pkg.data = frame.ap_member(net_frame_t, data);
            out_pkg.keep = -1;
            out_pkg.last = frp_item.ap_frp_last(net_frame_t);;

            net.write(out_pkg);
        }
    }
}


template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void ws_distribute_package( frp_stream_t        &in,
                            frp_stream_t        &out1,
                            frp_stream_t        &out2
                        )
{
#pragma HLS interface ap_ctrl_none port=return
    ap_uint<1> schedule_flag  = 0;

work_stealing: while (true)
    {
        frp_t  frp_item;
        if (in.read_nb(frp_item))
        {

            ap_uint<1> out1_full = (out1.full());
            ap_uint<1> out2_full = (out2.full());
            ap_uint<3> code;

            code.range(0, 0) = schedule_flag;
            code.range(1, 1) = out1_full;
            code.range(2, 2) = out2_full;

            switch (code)
            {

            case 0b111:
            case 0b101:
            case 0b100:
            case 0b001:
            {
                out1.write(frp_item);
                schedule_flag = 0;
                break;
            }
            case 0b110:
            case 0b011:
            case 0b010:
            case 0b000:
            {
                out2.write(frp_item);
                schedule_flag = 1;
                break;
            }
            }
        }
    }
}




template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void distribute_package(    frp_stream_t        &in,
                            frp_stream_t        &out1,
                            frp_stream_t        &out2,
                            const route_id_t    id1,
                            const route_id_t    id2,
                            const route_id_t    mask)
{
#pragma HLS interface ap_ctrl_none port=return
distribute_package_loop: while (true)
    {
        frp_t  frp_item;
        if (in.read_nb(frp_item))
        {
            route_id_t  dest = frp_item.ap_member(struct_t, dest);
            if ((dest & mask) == id1)
                out1.write(frp_item);
            else if ((dest & mask) == id2)
                out2.write(frp_item);
        }
    }
}

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void merge_package_with_priority(       frp_stream_t        &in1,
                                        frp_stream_t        &in2,
                                        frp_stream_t        &out)
{
#pragma HLS interface ap_ctrl_none port=return

    ap_uint<1> schedule_flag  = 0;

#ifdef SWITCH_DEBUG
    uint32_t counter = 0;
#endif

merge_package_loop: while (true)    {
#pragma HLS pipeline  II=1

        ap_uint<1> in1_empty = in1.empty();
        ap_uint<1> in2_empty = in2.empty();
        ap_uint<3> code;
        code.range(0, 0) = schedule_flag;
        code.range(1, 1) = in1_empty;
        code.range(2, 2) = in2_empty;

        switch (code)
        {
        case 0b111: break;
        case 0b110: break;
        case 0b101:
        case 0b100:
        case 0b001:
        case 0b000:
        {
            frp_t  frp_item;
            frp_item = in1.read();
            out.write(frp_item);
            schedule_flag = 0;
            break;
        }
        case 0b011:
        case 0b010:

        {
            frp_t  frp_item;
            frp_item = in2.read();
            out.write(frp_item);
            schedule_flag = 1;
            break;
        }
        }
    }
}




template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void merge_package(         frp_stream_t        &in1,
                            frp_stream_t        &in2,
                            frp_stream_t        &out)
{
#pragma HLS interface ap_ctrl_none port=return

    ap_uint<1> schedule_flag  = 0;

#ifdef SWITCH_DEBUG
    uint32_t counter = 0;
#endif

merge_package_loop: while (true)    {
#pragma HLS pipeline  II=1

        ap_uint<1> in1_empty = in1.empty();
        ap_uint<1> in2_empty = in2.empty();
        ap_uint<3> code;
        code.range(0, 0) = schedule_flag;
        code.range(1, 1) = in1_empty;
        code.range(2, 2) = in2_empty;

        switch (code)
        {
        case 0b111: break;
        case 0b110: break;
        case 0b101:
        case 0b100:
        case 0b001:
        {
            frp_t  frp_item;
            frp_item = in1.read();
#ifdef SWITCH_DEBUG
            frp_item.range(31, 0) = counter;
            counter++;
#endif
            out.write(frp_item);
            schedule_flag = 0;
            break;
        }
        case 0b011:
        case 0b010:
        case 0b000:
        {
            frp_t  frp_item;
            frp_item = in2.read();
#ifdef SWITCH_DEBUG
            frp_item.range(31, 0) = counter;
            counter++;
#endif
            out.write(frp_item);
            schedule_flag = 1;
            break;
        }
        }
    }
}



template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void filter_to_other_node(  frp_stream_t       &in,
                            frp_stream_t       &out,
                            frp_stream_t       &to_other,
                            const uint16_t     filtered_id,
                            const uint16_t     nbits_of_local )
{
#pragma HLS interface ap_ctrl_none port=return
filter: while (true)
    {
        frp_t frp_item;
        if (in.read_nb(frp_item))
        {
            route_id_t  dest = frp_item.ap_member(struct_t, dest);
            if ((dest >> nbits_of_local) == filtered_id) {
                to_other.write(frp_item);
            }
            else {
                out.write(frp_item);
            }
        }
    }
}

#define ap_merge_first(N)        range((N) - 1        ,           0)
#define ap_merge_second(N)       range((N) * 2 - 1    ,         (N))
#define ap_merge_size(N)         range((N) * 2 + 8 - 1,     (N) * 2)
#define ap_merge_last(N)         range((N) * 2 + 8    , (N) * 2 + 8)

template <typename frp_in, typename  frp_out>
void coalesce_with_bubble(  hls::stream < frp_in >          &in1,
                            hls::stream < frp_in >          &in2,
                            hls::stream < frp_out >         &out,
                            const uint32_t                  N)
{

#pragma HLS interface ap_ctrl_none port=return

    ap_uint<1> schedule_flag  = 0;

coalesce_with_bubble_loop: while (true)    {
#pragma HLS pipeline  II=1

        ap_uint<1> in1_empty = in1.empty();
        ap_uint<1> in2_empty = in2.empty();
        ap_uint<3> code;
        code.range(0, 0) = schedule_flag;
        code.range(1, 1) = in1_empty;
        code.range(2, 2) = in2_empty;

        switch (code)
        {
        case 0b111:
        case 0b110:
        {
            break;
        }
        case 0b101:
        case 0b100:
        {
            frp_in frp_item;
            frp_item = in1.read();
            schedule_flag = 0;

            frp_out out_data;
            out_data.ap_merge_first(N) = frp_item.ap_merge_first(N);
            out_data.ap_merge_second(N) = 0;
            out_data.ap_merge_size(N) = 0;
            out_data.ap_merge_last(N) = frp_item.range(N, N);
            out.write(out_data);

            break;
        }
        case 0b011:
        case 0b010:
        {
            frp_in frp_item;
            frp_item = in2.read();
            schedule_flag = 1;

            frp_out out_data;
            out_data.ap_merge_first(N) = frp_item.ap_merge_first(N);
            out_data.ap_merge_second(N) = 0;
            out_data.ap_merge_size(N) = 0;
            out_data.ap_merge_last(N) = frp_item.range(N, N);
            out.write(out_data);

            break;
        }
        case 0b001:
        case 0b000:
        {
            frp_in frp_item1 = in1.read();
            frp_in frp_item2 = in2.read();

            frp_out out_data;
            out_data.ap_merge_first(N)  = frp_item1.ap_merge_first(N);
            out_data.ap_merge_second(N) = frp_item2.ap_merge_first(N);
            out_data.ap_merge_size(N)   = 1;
            out_data.ap_merge_last(N)   = frp_item1.range(N, N) | frp_item2.range(N, N);
            out.write(out_data);

            break;
        }

        }
    }
}


//TODO: need a type class
template <typename frp_in, typename  frp_out, typename l1_item>
void dispatch_remote_package(  hls::stream < frp_in >          &in,
                               hls::stream < frp_out >         (&out)[4],
                               const uint32_t                  N)
{

#pragma HLS interface ap_ctrl_none port=return


remote: while (true)    {
#pragma HLS pipeline  II=1

        frp_in frp_item;
        if (in.read_nb(frp_item))
        {
            ap_uint<8> l1_size = frp_item.ap_merge_size((N * 2) + 8);
            {
                l1_item first =  frp_item.ap_merge_first((N * 2) + 8);
                ap_uint<8> l2_size  = first.ap_merge_size(N);
                frp_out out_item1 =  first.ap_merge_first(N);
                out[0].write(out_item1);
                if (l2_size & 0x1) {
                    frp_out out_item2 =  first.ap_merge_second(N);
                    out[1].write(out_item2);
                }
            }

            if (l1_size & 0x1) {
                l1_item second =  frp_item.ap_merge_second((N * 2) + 8);
                ap_uint<8> l2_size  = second.ap_merge_size(N);
                frp_out out_item1 =  second.ap_merge_first(N);
                out[2].write(out_item1);
                if (l2_size & 0x1) {
                    frp_out out_item2 =  second.ap_merge_second(N);
                    out[3].write(out_item2);
                }
            }
        }
    }
}

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void free_run_frp_to_net_timeout(    frp_stream_t    &frp,
                                     net_stream_t    &net,
                                     const int dest,
                                     const int timeout,
                                     const int frame_size)
{
#pragma HLS interface ap_ctrl_none port=return
    ap_uint<1> valid_flag  = 0;
    ap_uint<8> timeout_counter = 0;
    ap_uint<8> frame_counter = 0;

    frp_t  in_data;
frp_net: while (true)
    {
        ap_uint<1> in_empty = frp.empty();
        if (in_empty)
        {
            if (valid_flag == 1) {
                if (timeout_counter >= timeout) {
                    net_pkg_t out_pkg;
                    out_pkg.data = 0;
                    out_pkg.data = in_data;
                    out_pkg.data.range(511, 511) = 1; // debug
                    out_pkg.keep = -1;
                    out_pkg.last = 1;
                    out_pkg.dest = dest;
                    net.write(out_pkg);
                    valid_flag = 0;
                    frame_counter = 0;
                }
                else {
                    timeout_counter ++;
                }
            }
            else {
                continue;
            }
        }
        else
        {
            frp_t item = frp.read();
            timeout_counter  = 0;
            if (valid_flag == 0)
            {
                in_data = item;
                valid_flag = 1;
            }
            else
            {
                net_pkg_t out_pkg;
                out_pkg.data = 0;
                out_pkg.data = in_data;
                out_pkg.keep = -1;
                if (frame_counter == (frame_size - 1)) {
                    out_pkg.last = 1;
                    frame_counter = 0;
                }
                else {
                    out_pkg.last = 0;
                    frame_counter ++;
                }
                out_pkg.dest = dest;
                net.write(out_pkg);
                in_data = item;
            }
        }
    }
}


//TODO: no last single
template <typename frp_single , typename frp_pair>
void filter_bubble_in_pair( hls::stream < frp_pair >         &in,
                            hls::stream < frp_pair >         &out,
                            const uint32_t                 N,
                            const uint32_t                 timeout)
{

#pragma HLS interface ap_ctrl_none port=return

    ap_uint<1> valid_flag  = 0;
    ap_uint<8> timeout_counter = 0;

    frp_single buffer;

filter_bubble_loop: while (true)    {
#pragma HLS pipeline  II=1

        ap_uint<1> in_empty = in.empty();
        if (in_empty)
        {
            if (valid_flag == 1)
            {
                if (timeout_counter >= timeout) {
                    frp_pair out_data;
                    out_data.ap_merge_first(N)  = buffer.ap_merge_first(N);
                    out_data.ap_merge_second(N) = 0;
                    out_data.ap_merge_size(N)   = 0;
                    out_data.ap_merge_last(N)   = 0;
                    out.write(out_data);
                    valid_flag = 0;
                }
                else {
                    timeout_counter ++;
                }
            }
        }
        else
        {
            frp_pair item = in.read();
            timeout_counter = 0;
            ap_uint<8> total_size = item.ap_merge_size(N);
            ap_uint<1> full_flag = total_size & 0x1;

            if ((full_flag == 0) && (valid_flag == 0)) {
                buffer = item.ap_merge_first(N);

                valid_flag  = 1;
            }
            else if ((full_flag == 0) && (valid_flag == 1)) {
                frp_pair out_data;
                out_data.ap_merge_first(N)  = buffer.ap_merge_first(N);
                out_data.ap_merge_second(N) = item.ap_merge_first(N);
                out_data.ap_merge_size(N)   = 1;
                out_data.ap_merge_last(N)   = 0;
                out.write(out_data);

                valid_flag  = 0;
            }
            else if ((full_flag == 1) && (valid_flag == 0)) {
                frp_pair out_data;
                out_data = item;
                out.write(out_data);
                valid_flag  = 0;
            }
            else if ((full_flag == 1) && (valid_flag == 1)) {
                frp_pair out_data;
                out_data.ap_merge_first(N)  = buffer.ap_merge_first(N);
                out_data.ap_merge_second(N) = item.ap_merge_first(N);
                out_data.ap_merge_size(N)   = 1;
                out_data.ap_merge_last(N)   = 0;
                out.write(out_data);

                buffer = item.ap_merge_second(N);

                valid_flag  = 1;
            }


        }
    }
}




#include "butterfly_switch.hpp"
#include "butterfly_work_stealing.hpp"


#endif /* __BUTTERFLY_NETWORK_HPP__ */