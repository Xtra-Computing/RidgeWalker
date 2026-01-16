#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "stream_type.h"


extern "C" {
    void query_statistic( ap_uint<64>                     *mem,
                          debug_msg_stream_t              &state
                        )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_write_outstanding=1  max_write_burst_length=2

        ap_uint<32>  counter = 0;
#define OUTER_COUNTER (8192)

        uint64_t last_data = 0;
        while (1)
        {

            debug_msg_pkg_t pkg = state.read();
            uint64_t data = pkg.data;

            if (counter  <  OUTER_COUNTER) {
                counter ++;
            } else {
                counter = 0;
            }


            if (counter == (OUTER_COUNTER - 1)) {
                if (data == last_data) {
                    break;
                }
                else {
                    last_data = data;
                }
            }

        }

        mem[0] = last_data;

    }
}

