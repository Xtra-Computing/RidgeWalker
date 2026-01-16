#ifndef __HELPER_H__
#define __HELPER_H__

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)





#ifdef USE_LOG
#include "log.h"
#define user_printf  log_info
#else
#define user_printf  printf
#endif

#define TRACE()  {user_printf("[TRACE] %s@%d %s", __FUNCTION__, __LINE__, __FILE__); fflush(stdout);}

#ifndef DEBUG_PRINTF

#define DEBUG_TRACE()  {user_printf("[TRACE] %s@%d %s", __FUNCTION__, __LINE__, __FILE__); fflush(stdout);}
#define DEBUG_PRINTF(fmt,...)   {user_printf(fmt,##__VA_ARGS__); fflush(stdout);}
#define DEBUG_ERROR(fmt,...)    {user_printf(fmt,##__VA_ARGS__); fflush(stdout); exit(-1);}

#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) sizeof(arr)/sizeof((arr)[0])
#endif

#ifndef FLAG_SET
#define FLAG_SET                (1u)
#endif

#ifndef FLAG_RESET
#define FLAG_RESET              (0u)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) sizeof(arr)/sizeof((arr)[0])
#endif


#ifndef SIZE_ALIGNMENT
inline long unsigned int get_aligned_size(unsigned int in, unsigned int align)
{
    if (in == 0)
    {
        return align;
    }
    else
    {
        return (((((in - 1) / align) + 1) * align));
    }
}
#define SIZE_ALIGNMENT(in,align)    get_aligned_size((unsigned int)in,(unsigned int)align)
#endif


double getCurrentTimestamp(void);


#include <random>
#include <algorithm>

template<typename T> static void shuffle(T* src, uint32_t n, bool debug_mode = true) {
    std::random_device rd;
    std::mt19937 rng(debug_mode ? 0 : rd());
    std::shuffle(src, src + n, rng);
}



void segv_handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}



#endif /* __HELPER_H__ */
