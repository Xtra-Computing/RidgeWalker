#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rng_type.h"

#include <ap_int.h>



extern "C" {
    void rn_cmd_gen(uint32_t size,
                    uint32_t in_seed,
                    mem_access_cmd_stream_t &output
                   )
    {
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = in_seed bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

        minRand(in_seed, 1);
        for (int i = 0 ; i < size; i++)
        {
#pragma HLS PIPELINE II=1
            uint32_t seed = minRand(31, 0);
            ap_uint<32> one_read = seed & ((1024u * 512u) - 1u);
            mem_access_cmd_pkg_t  cmd_pkg;
            mem_access_cmd_item_t  cmd;

            ap_uint<1> lsb = one_read.range(1,1);

#define BIT_OF_SWAP              (1)
            one_read.range(1,1) = one_read(BIT_OF_SWAP, BIT_OF_SWAP);
            one_read.range(BIT_OF_SWAP,BIT_OF_SWAP) = lsb;


            cmd.ap_member(mem_access_cmd_t, addr)  = one_read;

            if (i == size - 1)
                cmd.ap_member(mem_access_cmd_t, state) = MM_STATE_MASK_END;
            else
                cmd.ap_member(mem_access_cmd_t, state) = 0;

            cmd_pkg.data = cmd;
            cmd_pkg.last = 0;
            output.write(cmd_pkg);
        }
    }

}

