#ifndef __VCACHE_SWITCH_TYPE_H__
#define __VCACHE_SWITCH_TYPE_H__

#include "stream_type.h"

#include "switch_data_struct.h"


typedef struct __attribute__((packed)){
    route_id_t dest;
    rpa_task_t data;
}  route_vertex_t;


GEN_STRUCT_INTERFACE(route_vertex)


#endif /* __VCACHE_SWITCH_TYPE_H__ */
