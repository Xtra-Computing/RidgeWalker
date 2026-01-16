#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rng_type.h"
#include "init_state.h"

#include "stream_operation.h"

#include "sou_function.h"

#define NRNG (8)

#define KERNEL_ID  (1 * NRNG  - 1)


#define REPRODUCT_DEBUG  (0)

void leaf_state_stream( rng_state_inner_stream_t &in,
                        rng_state_inner_stream_t &out,
                        rng_output_inner_stream_t &inner_out,
                        const int local_id)
{
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS function_instantiate variable=local_id
    const int id = KERNEL_ID + local_id;
    while (1) {
        rng_state_t state = in.read();
        out.write(state);
        rng_state_t update = multi_stream(state,  54u  + id);
        rng32_t leaf_state = output_function(update);
        inner_out.write(leaf_state);
    }
}

void instance(  rng_output_inner_stream_t &s_in,
                rng_output_inner_stream_t &out,
                const int local_id)
{
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS function_instantiate variable=local_id
    const int id = KERNEL_ID + local_id;
    const rng32_t tmp = tmp_xor32_s[id];
    rng32_t sg_state = tmp;
rng_out: while (1) {

        rng32_t sg = xorshiftjump(sg_state);
        sg_state = sg;
        rng32_t state = s_in.read();
        rng32_t output_rng = (sg ^ state);
        out.write_nb(output_rng); //non-blocking on fifo
    }
}

void instance_xor_only( rng_output_inner_stream_t &out,
                        const int local_id)
{
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS function_instantiate variable=local_id

    const int id = KERNEL_ID + local_id;
    const rng32_t tmp = tmp_xor32_s[id];
    rng32_t sg_state = tmp;
rng_out: while (1) {

        rng32_t sg = xorshiftjump(sg_state);
        sg_state = sg;
        rng32_t output_rng = sg;
        out.write(output_rng); //blocking for result reproduce
    }
}

extern "C" {
    void rng_instance(       rng_state_stream_t        &s_in,
                             rng_state_stream_t        &s_out,
                             rng_output_stream_t       &r_out1,
                             rng_output_stream_t       &r_out2,
                             rng_output_stream_t       &r_out3,
                             rng_output_stream_t       &r_out4,
                             rng_output_stream_t       &r_out5,
                             rng_output_stream_t       &r_out6,
                             rng_output_stream_t       &r_out7,
                             rng_output_stream_t       &r_out8)
    {

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        static rng_output_inner_stream_t rng_out[8];
#pragma HLS stream variable=rng_out  depth=2

#if REPRODUCT_DEBUG

        static rng_state_inner_stream_t inner_s[1];
#pragma HLS stream variable=inner_s  depth=2

        axis_to_hls_stream(s_in, inner_s[0]);
        hls_to_axis_stream(inner_s[0], s_out);

        for (int i = 0; i < 8 ; i ++ )
        {
#pragma HLS unroll
            instance_xor_only(rng_out[i], i);
        }

#else

        static rng_state_inner_stream_t inner_s[9];
#pragma HLS stream variable=inner_s  depth=2
        static rng_output_inner_stream_t leaf_state[8];
#pragma HLS stream variable=leaf_state  depth=2

        axis_to_hls_stream(s_in, inner_s[0]);
        for (int i = 0; i < 8 ; i ++ )
        {
#pragma HLS unroll
            leaf_state_stream(inner_s[i], inner_s[i + 1], leaf_state[i], i);
            instance(leaf_state[i], rng_out[i], i);
        }
        hls_to_axis_stream(inner_s[8], s_out);
#endif


        hls_to_axis_stream(rng_out[0], r_out1);
        hls_to_axis_stream(rng_out[1], r_out2);
        hls_to_axis_stream(rng_out[2], r_out3);
        hls_to_axis_stream(rng_out[3], r_out4);
        hls_to_axis_stream(rng_out[4], r_out5);
        hls_to_axis_stream(rng_out[5], r_out6);
        hls_to_axis_stream(rng_out[6], r_out7);
        hls_to_axis_stream(rng_out[7], r_out8);
    }
}
