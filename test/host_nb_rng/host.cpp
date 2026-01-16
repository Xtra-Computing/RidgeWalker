#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>

#include "rn_test.h"


#include "test_store.h"


#define NUM_OF_KERNEL  (8)


#define SIZE_FOR_TEST  (64 * 1024)

__attribute__((aligned(4096))) uint32_t test_buffer[SIZE_FOR_TEST];



int main(int argc, char *argv[]) {

    cl_accelerator acc(argc, argv);

    test_store      krnl_store[NUM_OF_KERNEL];


    for (int i = 0; i < NUM_OF_KERNEL ; i ++)
    {
        krnl_store[i] = test_store(acc,32, i);
        krnl_store[i].mem_transfer((void *)test_buffer, SIZE_FOR_TEST * sizeof(uint32_t), sizeof(uint32_t));
    }

    for (int i = 0; i < NUM_OF_KERNEL ; i ++)
    {
        krnl_store[i].async_schedule_task();
    }


    for (int i = 0; i < NUM_OF_KERNEL ; i ++)
    {
        krnl_store[i].async_join();
    }







    return 0;
}

