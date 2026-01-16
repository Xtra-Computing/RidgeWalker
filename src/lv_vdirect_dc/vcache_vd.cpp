#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rng_type.h"


#include "butterfly_network.hpp"

void vcache_vd_frp(     mem_access_stream_t(64)                     &in_data,
                        lquery_state_stream_t                       &in_qs,
                        sample_task_frp_inner_stream_t              &out
                  ) {


#pragma HLS interface ap_ctrl_none port=return

#pragma HLS interface axis register both port=in_data
#pragma HLS interface axis register both port=in_qs

    mem_access_pkg_t(64)   data_pkg;
    lquery_state_pkg_t     qs_pkg;

merge_main: while (1)
    {
        if (in_data.read_nb(data_pkg)) {
            qs_pkg = in_qs.read();
            sample_task_frp_item_t  st_frp;
            sample_task_item_t st_data;

            st_data.ap_sp_rp() = data_pkg.data.ap_mem_data(64);
            st_data.ap_sp_qs() = qs_pkg.data;

            st_frp.ap_frp_data(sample_task_t) = st_data;
            st_frp.ap_frp_last(sample_task_t) = qs_pkg.last;
            out.write(st_frp);
        }
    }
}



extern "C" {
    void vcache_vd(     mem_access_stream_t(64)         &in_data_0,
                        lquery_state_stream_t           &in_vds_0,
                        mem_access_stream_t(64)         &in_data_1,
                        lquery_state_stream_t           &in_vds_1,
                        sample_task_stream_t            &out_vd
                  ) {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

        sample_task_frp_inner_stream_t     inner_connection_g1[2];
#pragma HLS stream variable=inner_connection_g1  depth=13

        sample_task_frp_inner_stream_t     frp_out;
#pragma HLS stream variable=frp_out  depth=13


        vcache_vd_frp(in_data_0, in_vds_0, inner_connection_g1[0]);
        vcache_vd_frp(in_data_1, in_vds_1, inner_connection_g1[1]);

        merge_package< FRP_TEMPLATE(sample_task) >(
            inner_connection_g1[0],
            inner_connection_g1[1],
            frp_out
        );

        free_run_frp_to_axis_stream < FRP_TEMPLATE(sample_task) >(frp_out, out_vd);

    }
}