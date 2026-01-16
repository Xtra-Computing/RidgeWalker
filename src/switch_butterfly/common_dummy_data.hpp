#ifndef STRUCT_NAME
#error "STRUCT_NAME is not defined"
#endif

#ifndef FUNC_NAME
#error "FUNC_NAME is not defined"
#endif


#define  STREAM_TYPE_IMPL(name)    name##_stream_t
#define  PKG_TYPE_IMPL(name)       name##_pkg_t

#define  STREAM_TYPE(name)          STREAM_TYPE_IMPL(name)
#define  PKG_TYPE(name)             PKG_TYPE_IMPL(name)
