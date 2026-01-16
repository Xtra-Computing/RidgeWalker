#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "stream_operation.h"

extern "C" {
    void end_ctrl(      uint8_t value,
                        ctrl_stream_t  &out)
    {
        ctrl_pkg_t  pkg;
        pkg.data = value;
        pkg.last = 1;
        out.write(pkg);
        return;
    }
}
