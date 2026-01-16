#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"



#define  PERFORMANCE_TEST (1)



void sink_instance(uint32_t num_last, uint32_t nolast, uint32_t max_size, vertex_descriptor_stream_t &in, uint64_t &sum)
{

    uint32_t stop_counter = 0;
    uint32_t write_counter = 0;

    while (1)
    {

        vertex_descriptor_pkg_t pkg;
        if (in.read_nb(pkg)) {
            if (pkg.last == 1)
            {
                stop_counter++;
            }

            sum += pkg.data.ap_member(vertex_descriptor_t, start);

            write_counter ++;

            if (   ((write_counter >= max_size) && (nolast == 1))
                    || ((stop_counter == num_last)  && (nolast == 0))
               )
            {
                break;
            }
        }
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

                      vertex_descriptor_stream_t      &in1,
                      vertex_descriptor_stream_t      &in2,
                      vertex_descriptor_stream_t      &in3,
                      vertex_descriptor_stream_t      &in4,
                      vertex_descriptor_stream_t      &in5,
                      vertex_descriptor_stream_t      &in6,
                      vertex_descriptor_stream_t      &in7,
                      vertex_descriptor_stream_t      &in8

                    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem
#pragma HLS dataflow
        uint64_t array[8];
 #pragma HLS ARRAY_PARTITION variable=array   dim=0

        for (int i  =0 ; i < 8; i++)
        {
            array[i]  = 0 ;
        }

        sink_instance(num_last, nolast, max_size1, in1, array[1 - 1]);
        sink_instance(num_last, nolast, max_size2, in2, array[2 - 1]);
        sink_instance(num_last, nolast, max_size3, in3, array[3 - 1]);
        sink_instance(num_last, nolast, max_size4, in4, array[4 - 1]);
        sink_instance(num_last, nolast, max_size5, in5, array[5 - 1]);
        sink_instance(num_last, nolast, max_size6, in6, array[6 - 1]);
        sink_instance(num_last, nolast, max_size7, in7, array[7 - 1]);
        sink_instance(num_last, nolast, max_size8, in8, array[8 - 1]);

        for (int i  =0 ; i < 8; i++)
        {
            mem[i] =  array[i];
        }

    }
}

