#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>

#include "rng_type.h"
#include "rw_type.h"


void router_source_instance(uint32_t size,
                            uint32_t counter_a,
                            uint32_t counter_b,
                            uint32_t id,
                            uint32_t mask,
                            uint32_t in_seed,
                            uint32_t nolast,
                            next_vertex_stream_t   &output)
{
    minRand(in_seed, 1);
    uint32_t counter = 0;
    uint32_t sent_command = 0;
dummy_source: for (;;)
    {
        if ((counter <= counter_a) || (counter == (counter_b + counter_a)) || (counter == (counter_b * 2 + counter_a)))
        {
#pragma HLS PIPELINE II=1
            sent_command  ++;
            uint32_t rn = minRand(31, 0);
            uint32_t vid = rn & mask;

            next_vertex_pkg_t pkg;

            pkg.data.ap_member(next_vertex_t, vertex) = vid;

            pkg.last = ((sent_command == size) && (nolast == 0));
            output.write(pkg);
        }
        if (sent_command == size)
        {
            return;
        }

        if (counter == (counter_b * 2 + counter_a ))
        {
            counter = 0;
        }
        else
        {
            counter ++;
        }
    }
}


extern "C" {
    void router_source( uint32_t size,
                        uint32_t counter_a,
                        uint32_t counter_b,
                        uint32_t id,
                        uint32_t mask,
                        uint32_t in_seed,
                        uint32_t nolast,
                        next_vertex_stream_t   &output1,
                        next_vertex_stream_t   &output2,
                        next_vertex_stream_t   &output3,
                        next_vertex_stream_t   &output4,
                        next_vertex_stream_t   &output5,
                        next_vertex_stream_t   &output6,
                        next_vertex_stream_t   &output7,
                        next_vertex_stream_t   &output8
                      )
    {
#pragma HLS dataflow
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 1, nolast, output1);
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 2, nolast, output2);
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 3, nolast, output3);
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 4, nolast, output4);
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 5, nolast, output5);
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 6, nolast, output6);
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 7, nolast, output7);
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 8, nolast, output8);
    }
}

