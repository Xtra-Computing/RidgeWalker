#ifndef __VCLA_SWITCH_TYPE_H__
#define __VCLA_SWITCH_TYPE_H__

#include "stream_type.h"

#include "switch_data_struct.h"


typedef struct __attribute__((packed)) {
    route_id_t dest;
    cla_task_t data;
}  route_vertex_t;


typedef struct __attribute__((packed)) {
    uint8_t mask;
    rpa_task_t data[4];
}  route_net_vertex_t;


GEN_STRUCT_INTERFACE(route_vertex)

GEN_STRUCT_INTERFACE(route_net_vertex)



#endif /* __VCLA_SWITCH_TYPE_H__ */
