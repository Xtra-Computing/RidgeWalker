#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "test_common.h"

#include "rng_type.h"
#include "rw_type.h"




extern "C" {
    void rpa_rate_limiter  (
                        rpa_task_stream_t   &in1,
                        rpa_task_stream_t   &in2,
                        rpa_task_stream_t   &in3,
                        rpa_task_stream_t   &in4,
                        rpa_task_stream_t   &in5,
                        rpa_task_stream_t   &in6,
                        rpa_task_stream_t   &in7,
                        rpa_task_stream_t   &in8,

                        rpa_task_stream_t   &out1,
                        rpa_task_stream_t   &out2,
                        rpa_task_stream_t   &out3,
                        rpa_task_stream_t   &out4,
                        rpa_task_stream_t   &out5,
                        rpa_task_stream_t   &out6,
                        rpa_task_stream_t   &out7,
                        rpa_task_stream_t   &out8

                      ) {
#pragma HLS interface ap_ctrl_none port=return
        rate_limiter(0,in1, out1);
        rate_limiter(0,in2, out2);
        rate_limiter(0,in3, out3);
        rate_limiter(0,in4, out4);

        rate_limiter(0,in5, out5);
        rate_limiter(1,in6, out6);
        rate_limiter(0,in7, out7);
        rate_limiter(0,in8, out8);

    }
}
