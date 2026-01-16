#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"



#define  PERFORMANCE_TEST (1)




void state_instance(rpa_task_stream_t &in,
                    debug_msg_inner_stream_t &cnt)
{
#pragma HLS interface ap_ctrl_none port=return
    static uint64_t counter = 0;

    while (1)
    {
        rpa_task_pkg_t pkg;
        if (in.read_nb(pkg)) {
            counter ++;
        }
        debug_msg_item_t np_msg = counter;
        cnt.write_nb(np_msg);
    }

    return;
}


void debug_dump(  debug_msg_inner_stream_t (&in)[2], debug_msg_stream_t  &state_count )
{
#pragma HLS interface ap_ctrl_none port=return

    debug_msg_item_t dump_array[2];
#pragma HLS ARRAY_PARTITION variable=dump_array   dim=0


    while (1)
    {
        for (int i = 0; i < 2; i++)
        {
            dump_array[i] = in[i].read();
        }

        debug_msg_pkg_t  pkg;
        pkg.data = dump_array[0] + dump_array[1];
        pkg.last = 0;
        state_count.write(pkg);
    }

}

extern "C" {
    void router_state(

        rpa_task_stream_t      &in1,
        rpa_task_stream_t      &in2,

        debug_msg_stream_t     &state

    )
    {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow


        debug_msg_inner_stream_t debug_stream[2];
#pragma HLS stream variable=debug_stream  depth=2


        state_instance(in1, debug_stream[1 - 1]);
        state_instance(in2, debug_stream[2 - 1]);


        debug_dump (debug_stream, state);

    }
}




