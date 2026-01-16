#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rng_type.h"

#include "stream_operation.h"


void  sample_to_cmd(    sample_index_stream_t                   &input,
                        mem_access_cmd_stream_t                 &vertex_cmd)
{
#pragma HLS interface ap_ctrl_none port=return
    while (1)
    {
#pragma HLS PIPELINE II=1
        sample_index_pkg_t in_pkg;
        if (input.read_nb(in_pkg)) {
            uint64_t sample_index = in_pkg.data;
            next_vertex_item_t v;
            mem_access_cmd_item_t   cmd_item;
            ap_uint<32> cmd_value = 0;
            cmd_item.ap_member(mem_access_cmd_t, addr)  = sample_index;
            if ((sample_index & 0x80000000) == 0x80000000)
                cmd_value |= MM_STATE_MASK_DUMMY;
            else
                cmd_value &= (~MM_STATE_MASK_DUMMY);

            if (in_pkg.last == FLAG_SET)
                cmd_value |= MM_STATE_MASK_END;
            else
                cmd_value &= (~MM_STATE_MASK_END);

            cmd_item.ap_member(mem_access_cmd_t, state) = cmd_value;

            mem_access_cmd_pkg_t    cmd_pkg;
            cmd_pkg.data = cmd_item;
            cmd_pkg.last = in_pkg.last;
            vertex_cmd.write(cmd_pkg);

        }
    }
}


void  data_to_next_vertex(  mem_access_stream_t(32)                 &in_data,
                            next_vertex_stream_t                    &output)
{
#pragma HLS interface ap_ctrl_none port=return
    while (1)
    {
#pragma HLS PIPELINE II=1
        mem_access_pkg_t(32) data_pkg;
        if (in_data.read_nb(data_pkg))
        {
            next_vertex_pkg_t v_pkg;
            v_pkg.data = data_pkg.data.ap_mem_data(32);
            v_pkg.last = data_pkg.last;
            output.write(v_pkg);
        }
    }
}

extern "C" {
    void fetch_next(
        sample_index_stream_t                   &input,
        mem_access_cmd_stream_t                 &vertex_cmd,
        mem_access_stream_t(32)                 &in_data,
        next_vertex_stream_t                    &output
    )
    {
#pragma HLS interface ap_ctrl_none port=return


#pragma HLS dataflow
        sample_to_cmd(input, vertex_cmd);
        data_to_next_vertex(in_data, output);

    }

}


#if 0
static sample_index_inner_stream_t in;
#pragma HLS stream variable=in  depth=64


static next_vertex_inner_stream_t out;
#pragma HLS stream variable=out  depth=64


#pragma HLS dataflow
axis_to_hls_stream(input, in);
fetch_from_mem(in, vertex, out);
hls_to_axis_stream(out, output);



#define INNER_LOOP (3)
ap_uint<32> read_data[INNER_LOOP];
#pragma HLS ARRAY_PARTITION variable = read_data complete dim = 0
ap_uint<1> last_flag[INNER_LOOP];
#pragma HLS ARRAY_PARTITION variable = last_flag complete dim = 0

for (int i = 0; i < INNER_LOOP; i++)
{
    read_data[i] = 0;
    last_flag[i] = 0;
}
int counter = 0;
while (1)
{
    sample_index_pkg_t in_pkg = input.read();
    uint32_t sample_index = in_pkg.data;
    ap_uint<32> data = vertex[sample_index];

    for (int i = INNER_LOOP - 1; i >= 0; i--) {
        if (i == 0) {
            read_data[0] = data;
            last_flag[0] = in_pkg.last;
        } else {
            read_data[i] = read_data[i - 1];
            last_flag[i] = last_flag[i - 1];
        }
    }
    //if (counter == 0)
    {
        vertex_descriptor_pkg_t v;
        output.write(v);
        if (in_pkg.last)
        {
            return;
        }
    }
    //else
    {

    }
}
#endif