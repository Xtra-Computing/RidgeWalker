#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rng_type.h"


static const double inv_rand_max = 1.0 / ((double)0xffffffffu + 1.0);


extern "C" {
    void uniform_sample(    sample_task_stream_t            &input,
                            rng_output_stream_t             &rng,
                            cla_task_stream_t               &output
                         )
    {
#pragma HLS interface ap_ctrl_none port=return

        while (1) {
#pragma HLS PIPELINE II=1
            sample_task_pkg_t pkg;

            if (input.read_nb(pkg))
            {
                lquery_state_item_t qs = pkg.data.ap_sp_qs();
                lvertex_desp_item_t vd  = pkg.data.ap_sp_rp();
                ap_uint<27> degree = vd.ap_lvd_size();
                ap_uint<27> start  = vd.ap_lvd_addr();
                ap_uint<5>  channel_id  = vd.ap_lvd_chn();
                ap_uint<5>  node_id  = vd.ap_lvd_node();

                rng_output_pkg_t  rng_pkg = rng.read();
                uint32_t rn = rng_pkg.data;
                uint32_t selected_index =   (uint32_t)(((double)(degree)) * rn * inv_rand_max);


                //uint32_t sampled_index =  (uint32_t) (((double) rn / (0xffffffffu + 1.0)) * (degree + 1))

                ap_uint<27> sampled_index = selected_index + start;
                cla_task_pkg_t output_pkg;

                lcolumn_access_item_t  next_vertex_addr = 0;
                next_vertex_addr.ap_cl_addr() = sampled_index;
                next_vertex_addr.ap_cl_chn() =  channel_id;
                next_vertex_addr.ap_cl_node() =  node_id;
                output_pkg.data.ap_cla_qs() =  qs;
                output_pkg.data.ap_cla_ca() = next_vertex_addr;

                output.write(output_pkg);
            }
        }
    }
}
