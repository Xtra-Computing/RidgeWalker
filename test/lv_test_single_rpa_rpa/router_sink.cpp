#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"





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


extern "C" {
    void router_sink( ap_uint<128>          *mem,
                      uint32_t              size,
                      rpa_task_stream_t      &in
                    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_write_outstanding=256  max_write_burst_length=2


        uint32_t  counter = 0;

        while (1)
        {

            rpa_task_pkg_t pkg;
            if (in.read_nb(pkg)) {
                //mem[counter] = pkg.data;
                counter ++;
                if (counter >= size)
                {
                    break;
                }
            }
        }
        mem[0] = counter;

    }
}

