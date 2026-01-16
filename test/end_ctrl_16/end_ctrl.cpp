#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "stream_operation.h"

extern "C" {
    void end_ctrl(      uint8_t value,
                        ctrl_stream_t  &out_0_1,
                        ctrl_stream_t  &out_0_2,
                        ctrl_stream_t  &out_0_3,
                        ctrl_stream_t  &out_0_4,
                        ctrl_stream_t  &out_0_5,
                        ctrl_stream_t  &out_0_6,
                        ctrl_stream_t  &out_0_7,
                        ctrl_stream_t  &out_0_8,
                        ctrl_stream_t  &out_1_1,
                        ctrl_stream_t  &out_1_2,
                        ctrl_stream_t  &out_1_3,
                        ctrl_stream_t  &out_1_4,
                        ctrl_stream_t  &out_1_5,
                        ctrl_stream_t  &out_1_6,
                        ctrl_stream_t  &out_1_7,
                        ctrl_stream_t  &out_1_8)
    {
        ctrl_pkg_t  pkg;
        pkg.data = value;
        pkg.last = 1;

        out_0_1.write(pkg);
        out_0_2.write(pkg);
        out_0_3.write(pkg);
        out_0_4.write(pkg);
        out_0_5.write(pkg);
        out_0_6.write(pkg);
        out_0_7.write(pkg);
        out_0_8.write(pkg);
        out_1_1.write(pkg);
        out_1_2.write(pkg);
        out_1_3.write(pkg);
        out_1_4.write(pkg);
        out_1_5.write(pkg);
        out_1_6.write(pkg);
        out_1_7.write(pkg);
        out_1_8.write(pkg);

        return;
    }
}
