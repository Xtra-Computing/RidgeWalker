#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"



#define  PERFORMANCE_TEST (1)




void sink_instance(uint32_t num_last, uint32_t nolast, uint32_t max_size, rpa_task_stream_t &in,
                   debug_msg_inner_stream_t &out)
{

    uint32_t stop_counter = 0;
    uint64_t write_counter = 0;
    uint32_t timeout_counter = 0;
    uint64_t sum_value = 0;

    while (1)
    {

        rpa_task_pkg_t pkg;
        if (in.read_nb(pkg)) {

            sum_value += pkg.data.ap_sp_rp();

            write_counter ++;
            timeout_counter = 0;
        }
        else {
            timeout_counter ++;
        }

        if (timeout_counter > (max_size))
        {
            break;
        }
    }

    debug_msg_item_t np_msg =  write_counter;
    out.write(np_msg);
    debug_msg_item_t sum_msg = sum_value;
    out.write(sum_msg);

    return;
}


void debug_dump(  debug_msg_inner_stream_t (&in)[8], ap_uint<64>   *mem )
{
    debug_msg_item_t dump_array[16];
#pragma HLS ARRAY_PARTITION variable=dump_array   dim=0


    for (int i = 0; i < 8; i++)
    {
        dump_array[i] = in[i].read();
    }

    for (int i = 0; i < 8; i++)
    {
        dump_array[i + 8] = in[i].read();
    }

    for (int i = 0; i < 16; i ++)
    {
        mem[i] = dump_array[i];
    }


}

extern "C" {
    void router_sink( ap_uint<64>                     *mem,
                      uint32_t                        num_last,
                      uint32_t                        nolast,
                      uint32_t                        max_size1,
                      uint32_t                        max_size2,
                      uint32_t                        max_size3,
                      uint32_t                        max_size4,
                      uint32_t                        max_size5,
                      uint32_t                        max_size6,
                      uint32_t                        max_size7,
                      uint32_t                        max_size8,

                      rpa_task_stream_t      &in1,
                      rpa_task_stream_t      &in2,
                      rpa_task_stream_t      &in3,
                      rpa_task_stream_t      &in4,
                      rpa_task_stream_t      &in5,
                      rpa_task_stream_t      &in6,
                      rpa_task_stream_t      &in7,
                      rpa_task_stream_t      &in8

                    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_write_outstanding=1  max_write_burst_length=2
#pragma HLS dataflow

        debug_msg_inner_stream_t debug_stream[8];


        sink_instance(num_last, nolast, max_size1, in1, debug_stream[1 - 1]);
        sink_instance(num_last, nolast, max_size2, in2, debug_stream[2 - 1]);
        sink_instance(num_last, nolast, max_size3, in3, debug_stream[3 - 1]);
        sink_instance(num_last, nolast, max_size4, in4, debug_stream[4 - 1]);
        sink_instance(num_last, nolast, max_size5, in5, debug_stream[5 - 1]);
        sink_instance(num_last, nolast, max_size6, in6, debug_stream[6 - 1]);
        sink_instance(num_last, nolast, max_size7, in7, debug_stream[7 - 1]);
        sink_instance(num_last, nolast, max_size8, in8, debug_stream[8 - 1]);

        debug_dump (debug_stream, mem);

    }
}

