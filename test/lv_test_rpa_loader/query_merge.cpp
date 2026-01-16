#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"
#include "rw_type.h"


#define AHEAD_COUNTER (7168)

#define OLD_QUERY_TIMEOUT (4096 - 1)

void old_first_merge_instance(rpa_task_stream_t &old_query,
                              rpa_task_stream_t &new_query,
                              rpa_task_stream_t &out_query
                             )
{
#pragma HLS interface ap_ctrl_none port=return


    ap_uint<16> counter = AHEAD_COUNTER;
    ap_uint<16> timeout = 0;

merge_package_loop: while (true)    {
#pragma HLS pipeline  II=1

        rpa_task_pkg_t pkg;
        if (old_query.read_nb(pkg)) {
            rpa_task_pkg_t  updated_pkg;

            timeout = 0;

            lquery_state_item_t qs = pkg.data.ap_rpa_qs();
            if (qs.ap_qs_len() == 0) {
                if (counter < AHEAD_COUNTER) {
                    counter += 1;
                }

            }
            else {
                lquery_state_item_t new_qs;
                new_qs = qs;
                new_qs.ap_qs_len() = qs.ap_qs_len() - 1;
                updated_pkg.data.ap_rpa_vid() = pkg.data.ap_rpa_vid();
                updated_pkg.data.ap_rpa_qs()  = new_qs;
                updated_pkg.last = 0;
                out_query.write(updated_pkg);
            }


        }
        else {

            rpa_task_pkg_t  new_pkg;

            if (counter != 0)
            {
                if (new_query.read_nb(new_pkg)) {
                    out_query.write(new_pkg);
                    counter -= 1;

                }
            }
            if (timeout < OLD_QUERY_TIMEOUT) {
                timeout ++;
            }
            else {
                counter = AHEAD_COUNTER;
                timeout = 0;
            }


        }
    }
}



extern "C" {
    void query_merge(

        rpa_task_stream_t      &old1,
        rpa_task_stream_t      &old2,
        rpa_task_stream_t      &old3,
        rpa_task_stream_t      &old4,
        rpa_task_stream_t      &old5,
        rpa_task_stream_t      &old6,
        rpa_task_stream_t      &old7,
        rpa_task_stream_t      &old8,

        rpa_task_stream_t      &new1,
        rpa_task_stream_t      &new2,
        rpa_task_stream_t      &new3,
        rpa_task_stream_t      &new4,
        rpa_task_stream_t      &new5,
        rpa_task_stream_t      &new6,
        rpa_task_stream_t      &new7,
        rpa_task_stream_t      &new8,


        rpa_task_stream_t      &out1,
        rpa_task_stream_t      &out2,
        rpa_task_stream_t      &out3,
        rpa_task_stream_t      &out4,
        rpa_task_stream_t      &out5,
        rpa_task_stream_t      &out6,
        rpa_task_stream_t      &out7,
        rpa_task_stream_t      &out8

    )
    {
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow


        old_first_merge_instance(old1, new1, out1);
        old_first_merge_instance(old2, new2, out2);
        old_first_merge_instance(old3, new3, out3);
        old_first_merge_instance(old4, new4, out4);
        old_first_merge_instance(old5, new5, out5);
        old_first_merge_instance(old6, new6, out6);
        old_first_merge_instance(old7, new7, out7);
        old_first_merge_instance(old8, new8, out8);


    }
}




