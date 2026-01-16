#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>

#include "rng_type.h"
#include "rw_type.h"



#define PERFORMANCE_TEST (1)


void router_source_instance(uint32_t size,
                            uint32_t counter_a,
                            uint32_t counter_b,
                            uint32_t id,
                            uint32_t mask,
                            uint32_t in_seed,
                            uint32_t nolast,
                            rpa_task_stream_t   &output)
{
    minRand(in_seed, 1);
    uint8_t init_counter = 0;
    uint32_t sent_command = 0;
    uint32_t counter = 0;

dummy_source: for (;;)
    {
#pragma HLS PIPELINE II=1

#if PERFORMANCE_TEST
        if (1) {
#else
        if ((counter <= counter_a) || (counter == (counter_b + counter_a)) || (counter == (counter_b * 2 + counter_a))) {
#endif
            uint32_t rn = minRand(31, 0);
            uint32_t rn_id = rn & mask;

            rpa_task_pkg_t  pkg;
            lvertex_item_t     vertex;

            if (id != 0)
                vertex = id;
            else
                vertex = rn_id;

            lquery_state_item_t qs = 0;

            pkg.data.ap_rpa_vid() = vertex;
            pkg.data.ap_rpa_qs()  = qs;
            pkg.last = ((sent_command == size) && (nolast == 0));

            if (init_counter < 64)
            {
                init_counter ++;
            }
            else
            {
                sent_command  ++;
                output.write(pkg);
            }
        }
#if PERFORMANCE_TEST
        // generation logic for time irregular source.
#else
        if (counter == (counter_b * 2 + counter_a ))
        {
            counter = 0;
        }
        else
        {
            counter ++;
        }
#endif


        if (sent_command == size)
        {
            return;
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
                        rpa_task_stream_t   &output1,
                        rpa_task_stream_t   &output2
                      )
    {
#pragma HLS dataflow
#if PERFORMANCE_TEST
        router_source_instance(size, 1024, 0, id, mask, 0xffffff + 1, true, output1);
        router_source_instance(size, 1024, 0, id, mask, 0xffffff + 2, true, output2);

#else
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 1, nolast, output1);
        router_source_instance(size, counter_a, counter_b, id, mask, in_seed + 2, nolast, output2);
#endif
    }
}

