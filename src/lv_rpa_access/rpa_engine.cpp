#include "rpa_engine_instance.hpp"




extern "C" {
    void rpa_engine(    const ap_uint<64>          *data1,
                        const ap_uint<64>          *data2,
                        const ap_uint<64>          *data3,
                        const ap_uint<64>          *data4,
                        const ap_uint<64>          *data5,
                        const ap_uint<64>          *data6,
                        const ap_uint<64>          *data7,
                        const ap_uint<64>          *data8,
                        mem_access_cmd_stream_t    &in_cmd1,
                        mem_access_cmd_stream_t    &in_cmd2,
                        mem_access_cmd_stream_t    &in_cmd3,
                        mem_access_cmd_stream_t    &in_cmd4,
                        mem_access_cmd_stream_t    &in_cmd5,
                        mem_access_cmd_stream_t    &in_cmd6,
                        mem_access_cmd_stream_t    &in_cmd7,
                        mem_access_cmd_stream_t    &in_cmd8,
                        ctrl_stream_t              &in_ctrl1,
                        ctrl_stream_t              &in_ctrl2,
                        ctrl_stream_t              &in_ctrl3,
                        ctrl_stream_t              &in_ctrl4,
                        ctrl_stream_t              &in_ctrl5,
                        ctrl_stream_t              &in_ctrl6,
                        ctrl_stream_t              &in_ctrl7,
                        ctrl_stream_t              &in_ctrl8,
                        mem_access_stream_t(64)    &out_data1,
                        mem_access_stream_t(64)    &out_data2,
                        mem_access_stream_t(64)    &out_data3,
                        mem_access_stream_t(64)    &out_data4,
                        mem_access_stream_t(64)    &out_data5,
                        mem_access_stream_t(64)    &out_data6,
                        mem_access_stream_t(64)    &out_data7,
                        mem_access_stream_t(64)    &out_data8
                   )
    {
#pragma HLS dataflow
#pragma HLS INTERFACE m_axi port = data1 offset = slave bundle = gmem1 num_read_outstanding=64  max_read_burst_length=2 latency=118
#pragma HLS INTERFACE m_axi port = data2 offset = slave bundle = gmem2 num_read_outstanding=64  max_read_burst_length=2 latency=118
#pragma HLS INTERFACE m_axi port = data3 offset = slave bundle = gmem3 num_read_outstanding=64  max_read_burst_length=2 latency=118
#pragma HLS INTERFACE m_axi port = data4 offset = slave bundle = gmem4 num_read_outstanding=64  max_read_burst_length=2 latency=118
#pragma HLS INTERFACE m_axi port = data5 offset = slave bundle = gmem5 num_read_outstanding=64  max_read_burst_length=2 latency=118
#pragma HLS INTERFACE m_axi port = data6 offset = slave bundle = gmem6 num_read_outstanding=64  max_read_burst_length=2 latency=118
#pragma HLS INTERFACE m_axi port = data7 offset = slave bundle = gmem7 num_read_outstanding=64  max_read_burst_length=2 latency=118
#pragma HLS INTERFACE m_axi port = data8 offset = slave bundle = gmem8 num_read_outstanding=64  max_read_burst_length=2 latency=118


        rpa_engine_instance(data1, in_cmd1, in_ctrl1, out_data1);
        rpa_engine_instance(data2, in_cmd2, in_ctrl2, out_data2);
        rpa_engine_instance(data3, in_cmd3, in_ctrl3, out_data3);
        rpa_engine_instance(data4, in_cmd4, in_ctrl4, out_data4);
        rpa_engine_instance(data5, in_cmd5, in_ctrl5, out_data5);
        rpa_engine_instance(data6, in_cmd6, in_ctrl6, out_data6);
        rpa_engine_instance(data7, in_cmd7, in_ctrl7, out_data7);
        rpa_engine_instance(data8, in_cmd8, in_ctrl8, out_data8);

    }
}