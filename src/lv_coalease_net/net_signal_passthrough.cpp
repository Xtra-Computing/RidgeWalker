#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>


#include "rw_type.h"

#include "butterfly_network.hpp"

#include "stream_operation.h"


#include "router_coalease.hpp"




extern "C" {
    void net_signal_passthrough(

        net_signal_stream_t   &in,
        net_signal_stream_t   &out

    )
    {

#pragma HLS interface ap_ctrl_none port=return

        while (true) {
            net_signal_pkg_t pkg;
            if (in.read_nb(pkg)) {
                out.write(pkg);
            }
        }
    }
}





