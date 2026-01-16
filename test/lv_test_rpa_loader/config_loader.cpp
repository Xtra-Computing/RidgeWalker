#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "butterfly_network.hpp"

#include <ap_int.h>

#include "rng_type.h"
#include "rw_type.h"
#include "switch_type.h"

#define BURST_BUFFER_SIZE (2)


#define OUT_INTERFAEC(id)                   output##id


#define RPA_OUT(index, interface_index) {\
        int i = index; \
        rpa_task_pkg_t  pkg; \
        ap_uint<64> vid = raw.range(64 * (i + 1) - 1,  64 * (i)); \
        lvertex_item_t   vertex = vid.range(39, 0); \
        lquery_state_item_t qs = 0; \
        qs.ap_qs_len() = len; \
        qs.ap_qs_qid() = i + (q_index  << 3);\
        pkg.data.ap_rpa_vid() = vertex; \
        pkg.data.ap_rpa_qs()  = qs; \
        pkg.last = 0; \
        OUT_INTERFAEC(interface_index).write(pkg); \
    }




extern "C" {
    void config_loader(   ap_uint<512>  *mem,
                          int num_query_x8,
                          int len,
                          int mode,
                          rpa_task_stream_t   &output1,
                          rpa_task_stream_t   &output2,
                          rpa_task_stream_t   &output3,
                          rpa_task_stream_t   &output4,
                          rpa_task_stream_t   &output5,
                          rpa_task_stream_t   &output6,
                          rpa_task_stream_t   &output7,
                          rpa_task_stream_t   &output8,
                          router_mapper_stream_t  &map1,
                          router_mapper_stream_t  &map2

                      )
    {

        const int  max_burst_length =  BURST_BUFFER_SIZE;


        if (mode == 0) {

#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_read_outstanding=64 max_read_burst_length=max_burst_length
            for (int q_index = 0; q_index < num_query_x8; q_index ++)
            {
#pragma HLS PIPELINE II=1
                ap_uint<512> raw = mem[q_index];

                RPA_OUT(0, 1);
                RPA_OUT(1, 2);
                RPA_OUT(2, 3);
                RPA_OUT(3, 4);
                RPA_OUT(4, 5);
                RPA_OUT(5, 6);
                RPA_OUT(6, 7);
                RPA_OUT(7, 8);

            }
        }
        else if (mode == 10)
        {
            ap_uint<512> data = mem[0];

            for (int i = 0; i < 8; i ++)
            {
                router_mapper_item_t  config_item = data.ap_member_array(router_mapper_config_t, item, i);
                router_mapper_pkg_t pkg;
                pkg.data = config_item;
                pkg.last = 0;
                map1.write(pkg);
            }
        }
        else if (mode == 11)
        {
            ap_uint<512> data = mem[0];

            for (int i = 0; i < 8; i ++)
            {
                router_mapper_item_t  config_item = data.ap_member_array(router_mapper_config_t, item, i);
                router_mapper_pkg_t pkg;
                pkg.data = config_item;
                pkg.last = 0;
                map2.write(pkg);
            }
        }
        else
        {
            return;
        }

    }
}

