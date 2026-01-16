#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rng_type.h"


#include "butterfly_network.hpp"

void vcache_vd_frp(     mem_access_stream_t(64)                     &in_data,
                        vertex_descriptor_with_state_stream_t       &in_vds,
                        vertex_descriptor_frp_inner_stream_t        &out_vd
                  ) {


#pragma HLS interface ap_ctrl_none port=return

    mem_access_pkg_t(64)  data_pkg;
    vertex_descriptor_with_state_pkg_t vds_pkg;

merge_main: while (1)
    {
        if (in_data.read_nb(data_pkg)) {
            vds_pkg = in_vds.read();
            vertex_descriptor_frp_item_t  vd_frp;
            vd_frp.ap_frp_data(vertex_descriptor_t) = data_pkg.data.ap_mem_data(64);
            vd_frp.ap_frp_last(vertex_descriptor_t) = vds_pkg.last;
            out_vd.write(vd_frp);
        }
    }
}



extern "C" {
    void vcache_vd(     mem_access_stream_t(64)                     &in_data_0,
                        vertex_descriptor_with_state_stream_t       &in_vds_0,
                        mem_access_stream_t(64)                     &in_data_1,
                        vertex_descriptor_with_state_stream_t       &in_vds_1,
                        vertex_descriptor_stream_t                  &out_vd
                  ) {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        vertex_descriptor_frp_inner_stream_t     inner_connection_g1[2];
#pragma HLS stream variable=inner_connection_g1  depth=13

        vertex_descriptor_frp_inner_stream_t     frp_out;
#pragma HLS stream variable=frp_out  depth=13


        vcache_vd_frp(in_data_0, in_vds_0, inner_connection_g1[0]);
        vcache_vd_frp(in_data_1, in_vds_1, inner_connection_g1[1]);

        merge_package< FRP_TEMPLATE(vertex_descriptor) >(
            inner_connection_g1[0],
            inner_connection_g1[1],
            frp_out
        );

        free_run_frp_to_axis_stream < FRP_TEMPLATE(vertex_descriptor) >(frp_out, out_vd);

    }
}