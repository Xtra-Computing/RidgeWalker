#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"



#define  PERFORMANCE_TEST (1)




void state_instance(rpa_task_stream_t &in,
                    rpa_task_stream_t &out,
                    debug_msg_inner_stream_t &cnt)
{
#pragma HLS interface ap_ctrl_none port=return
    static uint64_t counter = 0;

    while (1)
    {
        rpa_task_pkg_t pkg;
        if (in.read_nb(pkg)) {
            counter ++;
            out.write(pkg);
            debug_msg_item_t np_msg = counter;
            cnt.write_nb(np_msg);
        }
    }

    return;
}


void debug_dump(  debug_msg_inner_stream_t (&in)[8], debug_msg_stream_t  &state_count )
{
#pragma HLS interface ap_ctrl_none port=return

    debug_msg_item_t dump_array[8];
#pragma HLS ARRAY_PARTITION variable=dump_array   dim=0

    debug_msg_item_t sum_l1[4];
#pragma HLS ARRAY_PARTITION variable=sum_l1   dim=0

    debug_msg_item_t sum_l2[2];
#pragma HLS ARRAY_PARTITION variable=sum_l2   dim=0

    while (1)
    {
        for (int i = 0; i < 8; i++)
        {
            dump_array[i] = in[i].read();
        }

        for (int i = 0; i < 4; i++)
        {
            sum_l1[i] = dump_array[2 * i] + dump_array[2 * i + 1];
        }

        for (int i = 0; i < 2; i ++)
        {
            sum_l2[i] = sum_l1[2 * i] + sum_l1[2 * i + 1];
        }
        debug_msg_pkg_t  pkg;
        pkg.data = sum_l2[0] + sum_l2[1];
        pkg.last = 0;
        state_count.write(pkg);
    }

}

extern "C" {
    void router_state(

        rpa_task_stream_t      &in1,
        rpa_task_stream_t      &in2,
        rpa_task_stream_t      &in3,
        rpa_task_stream_t      &in4,
        rpa_task_stream_t      &in5,
        rpa_task_stream_t      &in6,
        rpa_task_stream_t      &in7,
        rpa_task_stream_t      &in8,

        rpa_task_stream_t      &out1,
        rpa_task_stream_t      &out2,
        rpa_task_stream_t      &out3,
        rpa_task_stream_t      &out4,
        rpa_task_stream_t      &out5,
        rpa_task_stream_t      &out6,
        rpa_task_stream_t      &out7,
        rpa_task_stream_t      &out8,
        debug_msg_stream_t     &state

    )
    {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow


        debug_msg_inner_stream_t debug_stream[8];
#pragma HLS stream variable=debug_stream  depth=2


        state_instance(in1, out1, debug_stream[1 - 1]);
        state_instance(in2, out2, debug_stream[2 - 1]);
        state_instance(in3, out3, debug_stream[3 - 1]);
        state_instance(in4, out4, debug_stream[4 - 1]);
        state_instance(in5, out5, debug_stream[5 - 1]);
        state_instance(in6, out6, debug_stream[6 - 1]);
        state_instance(in7, out7, debug_stream[7 - 1]);
        state_instance(in8, out8, debug_stream[8 - 1]);

        debug_dump (debug_stream, state);

    }
}




