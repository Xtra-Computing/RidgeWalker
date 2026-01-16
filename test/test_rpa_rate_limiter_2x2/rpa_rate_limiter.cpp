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


                        rpa_task_stream_t   &out1,
                        rpa_task_stream_t   &out2


                      ) {
#pragma HLS interface ap_ctrl_none port=return
        rate_limiter(3,in1, out1);
        rate_limiter(7,in2, out2);

    }
}
