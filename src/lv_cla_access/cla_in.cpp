#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rng_type.h"
#include "stream_operation.h"


extern "C" {
    void cla_in(        cla_task_stream_t               &input,
                        //vertex_descriptor_item_t                *vertex,
                        lquery_state_stream_t           &vds_0,
                        mem_access_cmd_stream_t         &vertex_cmd_0,
                        lquery_state_stream_t           &vds_1,
                        mem_access_cmd_stream_t         &vertex_cmd_1
                      ) {
#pragma HLS interface ap_ctrl_none port=return




        while (1) {
#pragma HLS PIPELINE II=1
            cla_task_pkg_t cla_pkg;
            if (input.read_nb(cla_pkg)) {


                lcolumn_access_item_t cla_task = cla_pkg.data.ap_cla_ca();

                ap_uint<1> chn_id = (cla_task.ap_cl_dest()) & 0x01;
                uint32_t local_addr = cla_task.ap_cl_addr();

                lquery_state_pkg_t qs_pkg;
                qs_pkg.data = cla_pkg.data.ap_cla_qs();
                qs_pkg.last = cla_pkg.last;

                mem_access_cmd_item_t   cmd_item;
                cmd_item.ap_member(mem_access_cmd_t, addr) = local_addr; // access high 36 bit

                ap_uint<32> cmd_value = 0;
                if (cla_pkg.last == FLAG_SET)
                    cmd_value |= MM_STATE_MASK_END;

                cmd_item.ap_member(mem_access_cmd_t, state) = cmd_value;

                mem_access_cmd_pkg_t    cmd_pkg;
                cmd_pkg.data = cmd_item;
                cmd_pkg.last = 0;


                if (chn_id) {
                    vertex_cmd_1.write(cmd_pkg);
                    vds_1.write(qs_pkg);
                }
                else {
                    vertex_cmd_0.write(cmd_pkg);
                    vds_0.write(qs_pkg);
                }
            }
        }
    }
}
