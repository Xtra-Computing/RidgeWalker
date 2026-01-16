#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"

#include "stream_operation.h"


#include "router_coalease.hpp"




extern "C" {
    void router_coalease_p(

        rpa_task_stream_t    &in1,
        rpa_task_stream_t    &in2,
        rpa_task_stream_t    &in3,
        rpa_task_stream_t    &in4,
        rpa_task_stream_t    &in5,
        rpa_task_stream_t    &in6,
        rpa_task_stream_t    &in7,
        rpa_task_stream_t    &in8,


        net_stream_t         &rout1

    )
    {
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


#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

#define __TIMEOUT__  (8)
#define __NET_FRAME_SIZE__  (13)

        rpa_to_net_with_timeout(in1, coaleased_frame[0], 1, __NET_FRAME_SIZE__, __TIMEOUT__);
        rpa_to_net_with_timeout(in2, coaleased_frame[1], 2, __NET_FRAME_SIZE__, __TIMEOUT__);
        rpa_to_net_with_timeout(in3, coaleased_frame[2], 3, __NET_FRAME_SIZE__, __TIMEOUT__);
        rpa_to_net_with_timeout(in4, coaleased_frame[3], 4, __NET_FRAME_SIZE__, __TIMEOUT__);
        rpa_to_net_with_timeout(in5, coaleased_frame[4], 5, __NET_FRAME_SIZE__, __TIMEOUT__);
        rpa_to_net_with_timeout(in6, coaleased_frame[5], 6, __NET_FRAME_SIZE__, __TIMEOUT__);
        rpa_to_net_with_timeout(in7, coaleased_frame[6], 7, __NET_FRAME_SIZE__, __TIMEOUT__);
        rpa_to_net_with_timeout(in8, coaleased_frame[7], 8, __NET_FRAME_SIZE__, __TIMEOUT__);

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

        net_frame_out(l4_switch, rout1);

        //net_frame_out(l1_switch[1], rout2);
    }
}


