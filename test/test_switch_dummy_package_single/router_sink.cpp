#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"


#define  PERFORMANCE_TEST (1)
extern "C" {
    void router_sink( ap_uint<64>                     *mem,
                      uint32_t                        num_last,
                      uint32_t                        nolast,
                      uint32_t                        max_size,
                      vertex_descriptor_stream_t      &in

                    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem
        uint32_t stop_counter = 0;
        uint32_t write_counter = 0;
#if PERFORMANCE_TEST
        uint64_t sum = 0;
#endif
        while (1)
        {

            vertex_descriptor_pkg_t pkg;
            if (in.read_nb(pkg)) {
                if (pkg.last == 1)
                {
                    stop_counter++;
                }
#if PERFORMANCE_TEST
                sum += pkg.data.ap_member(vertex_descriptor_t, start);
#else
                mem[write_counter] = pkg.data;
#endif
                write_counter ++;

                if (   ((write_counter >= max_size) && (nolast == 1))
                        || ((stop_counter == num_last)  && (nolast == 0))
                   )
                {
#if PERFORMANCE_TEST
                    mem[0] = sum;
#endif
                    break;
                }
            }
        }
    }
}

