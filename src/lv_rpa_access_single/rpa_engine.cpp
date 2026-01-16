#include "rpa_engine_instance.hpp"




extern "C" {
    void rpa_engine(    const ap_uint<64>          *data1,
                        mem_access_cmd_stream_t    &in_cmd1,
                        ctrl_stream_t              &in_ctrl1,
                        mem_access_stream_t(64)    &out_data1
                   )
    {
#pragma HLS dataflow
#pragma HLS INTERFACE m_axi port = data1 offset = slave bundle = gmem1 num_read_outstanding=64  max_read_burst_length=2 latency=118


        rpa_engine_instance(data1, in_cmd1, in_ctrl1, out_data1);
    }
}