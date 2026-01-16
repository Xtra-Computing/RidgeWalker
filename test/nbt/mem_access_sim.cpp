#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rng_type.h"

#include <ap_int.h>



extern "C" {
    void mem_access_sim(uint32_t size,
                    uint32_t in_seed,
                    uint32_t counter_a,
                    uint32_t counter_b,
                    uint32_t mask,
                    uint32_t last,
                    mem_access_cmd_stream_t &output
                   )
    {
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = in_seed bundle = control
#pragma HLS INTERFACE s_axilite port = counter_a bundle = control
#pragma HLS INTERFACE s_axilite port = counter_b bundle = control
#pragma HLS INTERFACE s_axilite port = mask bundle = control
#pragma HLS INTERFACE s_axilite port = last bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

        minRand(in_seed, 1);
        uint32_t counter = 0;
        uint32_t sent_command = 0;
        for (;;)
        {
            if ((counter <= counter_a) || (counter == (counter_b + counter_a)) || (counter == (counter_b * 2 + counter_a)))
            {
#pragma HLS PIPELINE II=1
                uint32_t seed = minRand(31, 0);
                uint32_t one_read = seed & ((1024u * 512u) - 1u);
                mem_access_cmd_pkg_t  cmd_pkg;
                mem_access_cmd_item_t  cmd;
                cmd.ap_member(mem_access_cmd_t, addr)  = sent_command  | (one_read & 0x000f0000 );

                uint32_t cmd_state = 0;
                if(last)
                {
                    if (sent_command == size - 1)
                        cmd_state |= MM_STATE_MASK_END;
                }


                if (one_read & mask)
                    cmd_state |= MM_STATE_MASK_DUMMY;


                cmd.ap_member(mem_access_cmd_t, state) = cmd_state;

                cmd_pkg.data = cmd;
                cmd_pkg.last = 0;
                output.write(cmd_pkg);
                sent_command  ++;
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

