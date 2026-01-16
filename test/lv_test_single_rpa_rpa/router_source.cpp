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
    void router_source(
        ap_uint<128>            *mem,
        uint32_t                size,
        uint32_t                counter_a,
        uint32_t                counter_b,
        rpa_task_stream_t       &output
    )
    {
#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_read_outstanding=256  max_read_burst_length=32
        uint32_t counter = 0;
        uint32_t i = 0;
        minRand(0xa42, 1);

        ap_uint<128> mdata = mem[0];

        while (true)
        {
            if ((counter <= counter_a) || (counter == (counter_b + counter_a)) || (counter == (counter_b * 2 + counter_a))) {
                //ap_uint<128> mdata = mem[i];

                rpa_task_pkg_t  pkg;

                pkg.data = mdata;
                pkg.data.range(15,0) = i;
                pkg.last = (i == (size - 1));

                output.write(pkg);
                if (i == (size - 1))
                {
                    break;
                }
                i++;

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
}

