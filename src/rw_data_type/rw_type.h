#ifndef __RW_TYPE_H__
#define __RW_TYPE_H__


#include "stream_type.h"

#include "rw_data_struct.h"

#define INVALID_VERTEX (0xFFFFFFFF)

GEN_STRUCT_INTERFACE(assert)
GEN_STRUCT_INTERFACE(assert_mask)

GEN_STRUCT_INTERFACE(adjacent)

GEN_STRUCT_INTERFACE(column_list)

GEN_STRUCT_INTERFACE(rw_statistic)

GEN_STRUCT_INTERFACE(step_metadata)

GEN_STRUCT_INTERFACE(query_metadata)

GEN_STRUCT_INTERFACE(dummy_step)

GEN_STRUCT_INTERFACE(sample_index)

GEN_STRUCT_INTERFACE(weight_mask)

GEN_STRUCT_INTERFACE(mask_cmd)

GEN_STRUCT_INTERFACE(weight)

GEN_STRUCT_INTERFACE(burst_cmd)

GEN_STRUCT_INTERFACE(sync_id)

GEN_STRUCT_INTERFACE(burst_sync)
GEN_STRUCT_INTERFACE(burst_sync_with_state)
GEN_STRUCT_INTERFACE(burst_sync_padded)

GEN_STRUCT_INTERFACE(vertex_descriptor)
GEN_STRUCT_INTERFACE(vertex_descriptor_ordered)
GEN_STRUCT_INTERFACE(vertex_descriptor_with_state)
GEN_STRUCT_INTERFACE(vertex_descriptor_update)

GEN_STRUCT_INTERFACE(next_vertex)
GEN_STRUCT_INTERFACE(next_vertex_ordered)
GEN_STRUCT_INTERFACE(next_vertex_with_state)

GEN_STRUCT_INTERFACE(rs_mask)

GEN_STRUCT_INTERFACE(path)

GEN_STRUCT_INTERFACE(res_data)

GEN_STRUCT_INTERFACE(lcolumn_access)
GEN_STRUCT_INTERFACE(lvertex)
GEN_STRUCT_INTERFACE(lvertex_host)
GEN_STRUCT_INTERFACE(lvertex_desp)
GEN_STRUCT_INTERFACE(lquery_state)
GEN_STRUCT_INTERFACE(rpa_task)
GEN_STRUCT_INTERFACE(cla_task)
GEN_STRUCT_INTERFACE(query_task)
GEN_STRUCT_INTERFACE(sample_task)



#define ap_lv_ma_chid()    range(  1 - 1,  0)
#define ap_lv_dest()       range(  4 - 1,  1)

#define ap_lv_ma_vid()     range( 40 - 1,  4)

#define ap_lvd_addr()      range( 27 - 1,  0)
#define ap_lvd_size()      range( 54 - 1, 27)
#define ap_lvd_chn()       range( 59 - 1, 54)
#define ap_lvd_node()      range( 64 - 1, 59)

#define ap_qs_pv()         range( 40 - 1,  0)
#define ap_qs_qid()        range( 54 - 1, 40)
#define ap_qs_len()        range( 64 - 1, 54)

#define ap_rpa_vid()       range( 40 - 1,  0)
#define ap_rpa_qs()        range(104 - 1, 40)

#define ap_sp_rp()         range( 64 - 1,  0)
#define ap_sp_qs()         range(128 - 1, 64)


#define ap_cl_addr()       range( 27 - 1,  0)
#define ap_cl_chn()        range( 32 - 1, 27)
#define ap_cl_node()       range( 37 - 1, 32)
#define ap_cl_dest()       range( 37 - 1, 27)

#define ap_cla_ca()        range( 40 - 1,  0)
#define ap_cla_qs()        range(104 - 1, 40)

//   type          bit           max
//   vertex id     40            1T
//   edge list     40 * 12       480/512

// Single HBM channel max: 2^29 byte (512MB) --> 27 bit addressing

// Row pointer: 64 bit
//  27 bit local address   512M ddr
//  27 bit size            0.1B neighbours
//  10 bit channel/node id 1024 nodes
// Single board  1.717B edges


// Query state: 64 bit
//  40 bit previous_vertex
//  14 bit query_id
//  10 bit length

// Row pointer bus: 104bit //remote
//  40 bit vertex_id
//  64 bit query state
// ------
// dest   = vertex_id[2:0] (channel id)
// dc_id  = vertex_id[3:3]



// Processing bus: 168 bit //local
//  64 bit query state
//  64 bit row_pointer
//  40 bit current_vertex





#endif /* __RW_TYPE_H__ */
