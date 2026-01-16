#ifndef __RW_DATA_STRUCT__
#define __RW_DATA_STRUCT__


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

// Row pointer bus: 104bit
//  40 bit vertex_id
//  64 bit query state
// ------
// dest   = vertex_id[2:0] (channel id)
// dc_id  = vertex_id[3:3]


// Processing bus: 128 bit
//  64 bit row_pointer
//  64 bit query state


typedef struct __attribute__((packed)) {
    uint8_t value[5];
} lcolumn_access_t;

typedef struct __attribute__((packed)) {
    uint8_t value[8];
} lvertex_desp_t;


typedef struct __attribute__((packed)) {
    uint64_t addr:27;
    uint64_t size:27;
    uint64_t chn_id:8;
    uint64_t unused:2;
} lvertex_desp_host_t;



typedef struct __attribute__((packed)) {
    uint32_t l32_vid;
    uint8_t  h8_vid;
} lvertex_t;



typedef struct __attribute__((packed)) {
    uint64_t vid;
} lvertex_host_t;


typedef struct __attribute__((packed)) {
    lvertex_t previous_vertex;
    uint8_t   value[3];
} lquery_state_t;






typedef struct __attribute__((packed)) {
    lvertex_desp_t      vd;
    lquery_state_t      q;
} query_task_t;



typedef struct __attribute__((packed)) {
    lvertex_desp_t      vd;
    lquery_state_t      q;
} sample_task_t;


typedef struct __attribute__((packed)) {
    lvertex_t           v;
    lquery_state_t      q;
} rpa_task_t;


typedef struct __attribute__((packed)) {
    lcolumn_access_t     cl;
    lquery_state_t       q;
} cla_task_t;







typedef struct __attribute__((packed)) {
    uint32_t v;
}  adjacent_t;


typedef struct __attribute__((packed)) {
    uint32_t v;
}  column_list_t;


typedef struct __attribute__((packed)) {
    uint64_t counter;
}  rw_statistic_t;


typedef struct __attribute__((packed)) {
    uint32_t last_vertex;
    uint32_t curr_vertex;
} assert_t;

typedef struct __attribute__((packed)) {
    uint32_t curr_vertex;
    uint32_t remain_step;
} query_metadata_t;

typedef struct  __attribute__((packed)) {
    uint32_t curr_vertex;
    uint32_t remain_step;
    uint16_t state;
    uint16_t ring_state;
    uint32_t inner_state;
    uint32_t assert;
} step_metadata_t;


typedef struct __attribute__((packed)) {
    uint32_t curr_vertex;
    uint32_t remain_step;
    uint16_t state;
    uint16_t ring_state;
    uint32_t inner_state;
} res_data_t;


typedef struct  __attribute__((packed)) {
    uint32_t curr_vertex;
    uint32_t remain_step;
    uint16_t state;
    uint16_t ring_state;
    uint32_t inner_state;
    uint32_t assert;
    uint32_t pad[3];
} dummy_step_t;


typedef struct
{
    uint32_t curr_vertex;
    uint32_t inner_state;
} path_t;


typedef struct
{
    uint32_t start;
    uint32_t size;
} vertex_descriptor_t;

typedef uint32_t sample_index_t;

typedef struct
{
    uint64_t vertex;
} next_vertex_t;

typedef struct
{
    uint32_t vertex;
    uint16_t state;
} next_vertex_with_state_t;

typedef uint16_t  weight_mask_t;

typedef struct
{
    uint32_t start;
    uint32_t end;
} mask_cmd_t;

typedef struct
{
    uint32_t data[16];
} weight_t;


typedef struct
{
    uint64_t data[16];
} scaled_weight_t;


typedef struct __attribute__((packed))
{
    uint32_t addr;
    uint32_t size;
    uint32_t start;
    uint32_t end;
    uint16_t  id;
    uint16_t  burst_id;
    uint16_t  dynamic_id;
    uint16_t  state;
} burst_cmd_t;


typedef struct __attribute__((packed)) {
    uint16_t id;
} sync_id_t;




// TODO offset for sampling
typedef struct __attribute__((packed))
{
    uint32_t addr;
    uint16_t id;
    uint16_t burst_id;
    uint16_t dynamic_id;
    uint16_t state;
} burst_sync_t;


typedef struct __attribute__((packed))
{
    uint32_t addr;
    uint16_t id;
    uint16_t burst_id;
    uint16_t dynamic_id;
    uint16_t state;
    uint32_t assert;
} burst_sync_with_state_t;

typedef struct __attribute__((packed))
{
    uint32_t start;
    uint32_t size;
    uint16_t state;
} vertex_descriptor_with_state_t;

#define VDS_INVALID     (0x8000)

typedef struct __attribute__((packed))
{
    uint32_t start;
    uint32_t size;
    uint32_t v;
} vertex_descriptor_update_t;


typedef struct __attribute__((packed))
{
    uint32_t start;
    uint32_t size;
    uint16_t  id;
    uint16_t  burst_id;
    uint16_t  state;
} vertex_descriptor_ordered_t;

typedef struct __attribute__((packed))
{
    uint32_t vertex;
    uint16_t id;
} next_vertex_ordered_t;


typedef struct __attribute__((packed)) {
    uint16_t id;
} rs_mask_t;


typedef struct __attribute__((packed)) {
    uint8_t id;
} assert_mask_t;


typedef struct __attribute__((packed))
{
    uint32_t addr;
    uint16_t id;
    uint16_t burst_id;
    uint16_t dynamic_id;
    uint16_t padding[3];
} burst_sync_padded_t;



#endif /* __RW_DATA_STRUCT__ */
