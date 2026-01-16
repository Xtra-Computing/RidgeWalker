#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"







extern "C" {
    void net_sink( ap_uint<128>          *mem,
                   uint32_t              size,
                   net_stream_t          &in
                 )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_write_outstanding=2  max_write_burst_length=2


        uint32_t  counter = 0;

        while (1)
        {

            net_pkg_t pkg;
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

