#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>

#include "rng_type.h"


extern "C" {
    void router_source( uint32_t size,
                        uint32_t counter_a,
                        uint32_t counter_b,
                        uint32_t id,
                        uint32_t mask,
                        uint32_t in_seed,
                        uint32_t nolast,
                        default_route_stream_t   &out
                      )
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
                uint32_t one_read = rn & ((1024u * 512u) - 1u);

                default_route_pkg_t pkg;
                if (id == 0)
                    pkg.data.ap_member(default_route_t, dest) = (rn) & mask;
                else
                    pkg.data.ap_member(default_route_t, dest) = mask;

                pkg.data.ap_member(default_route_t, data) = rn;

                pkg.last = ((sent_command == size) && (nolast == 0));
                out.write(pkg);

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
}

