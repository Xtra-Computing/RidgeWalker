#ifndef __SWITCH_DATA_STRUCT__
#define __SWITCH_DATA_STRUCT__


typedef uint8_t route_id_t;


typedef struct __attribute__((packed)){
    uint8_t ori_id;
    uint8_t mapped_id;
}  router_mapper_t;



typedef struct __attribute__((packed)){
   router_mapper_t item[8];
}  router_mapper_config_t;




typedef struct __attribute__((packed)){
    route_id_t dest;
    uint64_t data;
}  default_route_t;

typedef struct __attribute__((packed)){
    default_route_t data1;
    default_route_t data2;
    uint8_t reserved;
}  timeout_debug_t;


typedef struct __attribute__((packed)){
    default_route_t data[2];
    uint8_t reserved;
}  default_route_x2_t;


typedef struct __attribute__((packed)){
    default_route_x2_t data[2];
    uint8_t reserved;
}  default_route_x4_t;




#endif /* __SWITCH_DATA_STRUCT__ */
