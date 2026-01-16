#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "test_common.h"

#define BIT_WIDTH   (512)

extern "C" {
    void test_net_source  (
        net_stream_t             &rout

    ) {
        uint64_t counter = 0;
#pragma HLS interface ap_ctrl_none port=return
        while (true)
        {
            counter ++;
            if (counter == 0xfffffffffffffffelu)
            {
                net_pkg_t  net_pkg;
                net_pkg.data = 0;
                net_pkg.last = 1;
                rout.write(net_pkg);
            }

        }
    }
}
