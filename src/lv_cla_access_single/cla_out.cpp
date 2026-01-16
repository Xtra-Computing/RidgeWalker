#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rng_type.h"


#include "butterfly_network.hpp"

void cla_out_frp(       mem_access_stream_t(64)                     &in_data,
                        lquery_state_stream_t                       &in_qs,
                        rpa_task_frp_inner_stream_t                 &out
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

            rpa_task_frp_item_t  rpa_frp;
            rpa_task_item_t rpa_data;

            rpa_data.ap_rpa_vid() = data_pkg.data.ap_mem_data(64);
            rpa_data.ap_rpa_qs() = qs_pkg.data;

            rpa_frp.ap_frp_data(rpa_task_t) = rpa_data;
            rpa_frp.ap_frp_last(rpa_task_t) = qs_pkg.last;
            out.write(rpa_frp);
        }
    }
}



extern "C" {
    void cla_out(
                        mem_access_stream_t(64)         &in_data_1,
                        lquery_state_stream_t           &in_vds_1,
                        rpa_task_stream_t               &out_vd
                  ) {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow



        rpa_task_frp_inner_stream_t     frp_out;
#pragma HLS stream variable=frp_out  depth=13



        cla_out_frp(in_data_1, in_vds_1, frp_out);


        free_run_frp_to_axis_stream < FRP_TEMPLATE(rpa_task) >(frp_out, out_vd);

    }
}