#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rng_type.h"
#include "stream_operation.h"


extern "C" {
    void rpa_in(        rpa_task_stream_t               &input,
                        //vertex_descriptor_item_t                *vertex,
                        lquery_state_stream_t           &vds_1,
                        mem_access_cmd_stream_t         &vertex_cmd_1
               ) {
#pragma HLS interface ap_ctrl_none port=return




        while (1) {
#pragma HLS PIPELINE II=1
            rpa_task_pkg_t rpa_pkg;
            if (input.read_nb(rpa_pkg)) {


                lvertex_item_t v = rpa_pkg.data.ap_rpa_vid();

                ap_uint<1> chn_id = v.ap_lv_ma_chid();
                uint32_t local_vid = v.ap_lv_ma_vid();

                lquery_state_pkg_t qs_pkg;
                qs_pkg.data = rpa_pkg.data.ap_rpa_qs();
                qs_pkg.last = rpa_pkg.last;

                mem_access_cmd_item_t   cmd_item;
                cmd_item.ap_member(mem_access_cmd_t, addr)  = v; // single version

                ap_uint<32> cmd_value = 0;
                if (rpa_pkg.last == FLAG_SET)
                    cmd_value |= MM_STATE_MASK_END;

                cmd_item.ap_member(mem_access_cmd_t, state) = cmd_value;

                mem_access_cmd_pkg_t    cmd_pkg;
                cmd_pkg.data = cmd_item;
                cmd_pkg.last = 0;

                vertex_cmd_1.write(cmd_pkg);
                vds_1.write(qs_pkg);
            }
        }
    }
}
