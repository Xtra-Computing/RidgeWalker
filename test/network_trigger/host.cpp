#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

#include "graph.h"
#include "util/command_parser.h"
#include "log.h"




#include "netconfig.hpp"
#include "netbarrier.hpp"


static volatile int run_flag = 1;

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}



void int_handler(int dummy) {
    static uint32_t force_quit =0;
    run_flag = 0;
    printf("stop signal from user\n");
    force_quit ++;
    if (force_quit > 2){
        exit(1);
    }

}



int main(int argc, char *argv[]) {
    printf("[BUILD TIME]: %s %s\n", __DATE__, __TIME__);
    printf("[BUILD CONFIG]: %s \n", NET_CONFIG_NAME);

    signal(SIGSEGV, handler);   // install our handler

    signal(SIGINT, int_handler);

    net_barrier_trigger_init();

    DEBUG_TRACE();
    for (int i = 0; i < 4 ; i++){
        net_barrier_trigger();
    }
    DEBUG_TRACE();

    return 0;
}


