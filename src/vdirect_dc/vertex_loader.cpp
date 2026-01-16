#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"
#include "rng_type.h"
#include "stream_operation.h"


extern "C" {
    void vertex_loader( next_vertex_stream_t                      &input,
                        //vertex_descriptor_item_t                *vertex,
                        vertex_descriptor_with_state_stream_t     &vds_0,
                        mem_access_cmd_stream_t                   &vertex_cmd_0,
                        vertex_descriptor_with_state_stream_t     &vds_1,
                        mem_access_cmd_stream_t                   &vertex_cmd_1
                      ) {
#pragma HLS interface ap_ctrl_none port=return

        while (1) {
#pragma HLS PIPELINE II=1
            next_vertex_pkg_t nv_pkg;
            if (input.read_nb(nv_pkg)) {

                uint64_t v = nv_pkg.data.ap_member(next_vertex_t, vertex);
                vertex_descriptor_with_state_pkg_t v_pkg;

                v_pkg.data.ap_member(vertex_descriptor_with_state_t, start) = v;  // v is used for update
                v_pkg.data.ap_member(vertex_descriptor_with_state_t, size)  = 0; //size is checked in vcache data
                v_pkg.data.ap_member(vertex_descriptor_with_state_t, state) = VDS_INVALID;
                v_pkg.last = nv_pkg.last;

                mem_access_cmd_item_t   cmd_item;
                cmd_item.ap_member(mem_access_cmd_t, addr)  = v >> 1; // access in 64-bit

                ap_uint<32> cmd_value = 0;
                if (nv_pkg.last == FLAG_SET)
                    cmd_value |= MM_STATE_MASK_END;

                cmd_item.ap_member(mem_access_cmd_t, state) = cmd_value;

                mem_access_cmd_pkg_t    cmd_pkg;
                cmd_pkg.data = cmd_item;
                cmd_pkg.last = 0;


                if (v & 0x1) {
                    vertex_cmd_1.write(cmd_pkg);
                    vds_1.write(v_pkg);
                }
                else {
                    vertex_cmd_0.write(cmd_pkg);
                    vds_0.write(v_pkg);
                }
            }
        }
    }
}
