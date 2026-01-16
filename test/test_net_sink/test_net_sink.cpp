#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "test_common.h"

#define BIT_WIDTH   (512)

extern "C" {
    void test_net_sink  (
        net_stream_t             &rin

    ) {
#pragma HLS interface ap_ctrl_none port=return
        while (true)
        {
            net_pkg_t  net_pkg = rin.read();
        }
    }
}
