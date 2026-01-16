#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rw_type.h"




#define BURST_BUFFER_SIZE (32)

#define LOAD_FROM_GMEM  (1)

extern "C" {
    void step_loader(   query_metadata_item_t  *mem,
                        step_metadata_stream_t &new_query,
                        int size,
                        uint32_t prop)
    {

const int  max_burst_length =  BURST_BUFFER_SIZE;

#pragma HLS INTERFACE m_axi port=mem offset=slave bundle=gmem num_read_outstanding=2 max_read_burst_length=max_burst_length



#if LOAD_FROM_GMEM
        uint8_t counter =0;

        query_metadata_item_t buffer[BURST_BUFFER_SIZE];

        for (int i = 0; i < size; i+=BURST_BUFFER_SIZE) {

            for (int j = 0; j < BURST_BUFFER_SIZE; j++)
            {
                buffer[j] = mem[i + j];
            }
            for (int j = 0; j < BURST_BUFFER_SIZE; j++)
            {
                query_metadata_item_t a = buffer[j];
                step_metadata_pkg_t v;
                v.data.ap_member(step_metadata_t, curr_vertex) = a.ap_member(query_metadata_t, curr_vertex);
                v.data.ap_member(step_metadata_t, remain_step) = a.ap_member(query_metadata_t, remain_step);
                v.data.ap_member(step_metadata_t, inner_state) = i + j;
                v.data.ap_member(step_metadata_t, state) = 0;
                v.data.ap_member(step_metadata_t, ring_state) = 0;
                v.data.ap_member(step_metadata_t, assert) = prop;
                counter++;

                v.last = ((i + j) == (size - 1));
                new_query.write(v);
            }

        }

#else
        minRand(16807, 1);

        for (int i = 0; i < size; i++) {

            unsigned int rng = minRand(31, 0);
            unsigned int addr = (rng % size);

            step_metadata_pkg_t v;
            v.data.ap_member(step_metadata_t, curr_vertex) = addr;
            v.data.ap_member(step_metadata_t, remain_step) = 10;
            v.data.ap_member(step_metadata_t, state)       = 0;
            v.data.ap_member(step_metadata_t, inner_state) = i + j;
            v.data.ap_member(step_metadata_t, ring_state) = 0;
            v.data.ap_member(step_metadata_t, assert) = prop;
            v.last = (i == (size - 1));
            new_query.write(v);

        }
#endif
    }
}
