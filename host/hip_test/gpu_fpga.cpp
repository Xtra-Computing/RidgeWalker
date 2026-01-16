/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

#include "cmdlineparser.h"
#include <iostream>
#include <cstring>
#include <iomanip>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"
#include "experimental/xrt_queue.h"

#include <cstdio>  //fprintf
#include <iostream> //cerr
#include <unistd.h> //usleep

#include "hip/hip_runtime_api.h"

#include "hip/hip_runtime.h"

#define  __ZERO_COPY__ (1)




void check_error(void)
{
    hipError_t err = hipGetLastError();
    if (err != hipSuccess)
    {
        std::cerr << "Error: " << hipGetErrorString(err) << std::endl;
        exit(err);
    }
}





#define TBSIZE      (1024)
#define ARRAY_SIZE  (33554432/4)
#define BLOCK_SIZE  (1)
#define LOOP        (5)
#define SRC_VALUE   (7)
#define DST_VALUE   (2)

std::string getDeviceName(const int device)
{
    hipDeviceProp_t props;
    hipGetDeviceProperties(&props, device);
    check_error();
    return std::string(props.name);
}


std::string getDeviceDriver(const int device)
{
    hipSetDevice(device);
    check_error();
    int driver;
    hipDriverGetVersion(&driver);
    check_error();
    return std::to_string(driver);
}

#define type_t int

type_t *d_src1 = nullptr;
type_t *d_src2 = nullptr;
type_t *d_dst = nullptr;



template <typename T>
__global__ void copy_kernel(const T * d_src1, const T * d_src2, T * d_dst)
{
    //asm volatile ("buffer_wbinvl1_vol");
    const int i = hipBlockDim_x * hipBlockIdx_x + hipThreadIdx_x;


#if 1
#pragma unroll
    for  (int j = 0; j < BLOCK_SIZE; j++)
        d_dst[i * BLOCK_SIZE + j] = d_src1[i * BLOCK_SIZE + j]  + d_src2[i * BLOCK_SIZE + j];
#else
    const int id = ((i >> 10) & 1);
    if (id)
    {
        d_dst[i] = d_src1[i] * 2;
    }
    else
    {
        d_dst[i] = d_src2[i] ;
    }
#endif
}

template <class T>
void launch_copy(T *d_src1,  T *d_src2,  T *d_dst)
{
    hipLaunchKernelGGL(HIP_KERNEL_NAME(copy_kernel<T>), dim3(ARRAY_SIZE / TBSIZE / BLOCK_SIZE), dim3(TBSIZE), 0, 0, d_src1, d_src2,  d_dst);
    check_error();
    hipDeviceSynchronize();
    check_error();
}

template <typename T>
__global__ void init_kernel(T * d_src1, T * d_src2, T * d_dst, T v_src, T v_dst)
{
    asm volatile ("buffer_wbinvl1_vol");
    const int i = hipBlockDim_x * hipBlockIdx_x + hipThreadIdx_x;

    for  (int j = 0; j < BLOCK_SIZE; j++)
    {
        d_src1[i * BLOCK_SIZE + j] = v_src;
        d_src2[i * BLOCK_SIZE + j] = v_src;
        d_dst[i * BLOCK_SIZE + j] = v_dst;
    }
    //asm volatile ("buffer_wbinvl1_vol");
}

template <class T>
void init_arrays(T *d_src1, T *d_src2,  T *d_dst)
{
    T v_src = SRC_VALUE;
    T v_dst = DST_VALUE;
    hipLaunchKernelGGL(HIP_KERNEL_NAME(init_kernel<T>), dim3(ARRAY_SIZE / TBSIZE / BLOCK_SIZE), dim3(TBSIZE), 0, 0, d_src1, d_src2, d_dst, v_src, v_dst  );
    check_error();
    hipDeviceSynchronize();
    check_error();
}




template <class T>
void hip_init(T * host_addr1, T * host_addr2)
{
    unsigned int device_index = 1;
    // Set device
    int count;
    hipGetDeviceCount(&count);
    printf("number of devices: %d\n", count);
    check_error();
    if (device_index >= count)
        throw std::runtime_error("Invalid device index");
    hipSetDevice(device_index);
    check_error();

    hipSetDevice(device_index);
    check_error();

    // Print out device information
    std::cout << "Using HIP device " << getDeviceName(device_index) << std::endl;
    std::cout << "Driver: " << getDeviceDriver(device_index) << std::endl;

    if (host_addr1)
    {
        hipHostRegister((void*) host_addr1, ARRAY_SIZE * sizeof(T), hipHostRegisterMapped);
        check_error();
        hipHostGetDevicePointer( (void**) &d_src1, (void*) host_addr1, 0 );
        check_error();
    }
    else {
        hipMalloc(&d_src1, ARRAY_SIZE * sizeof(T));
        check_error();
    }


    if (host_addr2)
    {
        hipHostRegister((void*) host_addr2, ARRAY_SIZE * sizeof(T), hipHostRegisterMapped);
        check_error();
        hipHostGetDevicePointer( (void**) &d_src2, (void*) host_addr2, 0 );
        check_error();
    }
    else {
        hipMalloc(&d_src2, ARRAY_SIZE * sizeof(T));
        check_error();
    }



    hipMalloc(&d_dst, ARRAY_SIZE * sizeof(T));
    check_error();
}
template <class T>
void hip_deinit(T * host_addr1, T * host_addr2 )
{
    if (host_addr1)
    {
        hipHostUnregister((void*) host_addr1);
        check_error();
    }
    else {
        hipFree(d_src1);
        check_error();
    }

    if (host_addr2)
    {
        hipHostUnregister((void*) host_addr2);
        check_error();
    }
    else {
        hipFree(d_src2);
        check_error();
    }

    hipFree(d_dst);
    check_error();
}



type_t test_array1[ARRAY_SIZE * 16];
type_t test_array2[ARRAY_SIZE * 16];

int bufReference[ARRAY_SIZE];


void gpu_copy_benchmark(type_t * host_src1, type_t * host_src2)
{
    hip_init<type_t>(host_src1, host_src2);
    init_arrays<type_t>(d_src1, d_src2, d_dst);
    std::chrono::high_resolution_clock::time_point t1, t2;


    for (int i = 0; i < LOOP; i ++)
    {
        t1 = std::chrono::high_resolution_clock::now();
        launch_copy<type_t>(d_src1, d_src2, d_dst);
        t2 = std::chrono::high_resolution_clock::now();
        double exe_time = std::chrono::duration_cast <
                          std::chrono::duration<double> > (t2 - t1).count();
        if (host_src1) {
            printf("t_exe %lf, thr :%lf GB/s\n", exe_time,
                   sizeof(type_t) *ARRAY_SIZE / exe_time / 1000 / 1000 / 1000);
        }
        else {
            printf("t_exe %lf, thr :%lf GB/s\n", exe_time,
                   2 * sizeof(type_t) *ARRAY_SIZE / exe_time / 1000 / 1000 / 1000);
        }
    }

    std::vector<type_t> read_back(ARRAY_SIZE);
    hipMemcpy(read_back.data(), d_dst, read_back.size()*sizeof(type_t), hipMemcpyDeviceToHost);
    check_error();
    int error_count = 0;
    if (host_src1)
    {
        for (int i = 0; i < read_back.size(); i++)
        {
            if (read_back[i] != 2 * SRC_VALUE)
            {
                //printf("%d, %d\n", read_back[i], 2 * SRC_VALUE );
                //break;
                error_count++;
            }
            if (host_src1[i] != SRC_VALUE)
            {
                error_count++;
            }
        }
        printf("error count: %d\n", error_count);
    }

    hip_deinit<type_t>(host_src1, host_src2);

}

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input xclbin", "increment.xclbin");
    parser.addSwitch("--device_id1", "-d1", "device index", "1");
    parser.addSwitch("--device_id2", "-d2", "device index", "2");
    parser.parse(argc, argv);

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    int device_index1 = stoi(parser.value("device_id1"));
    int device_index2 = stoi(parser.value("device_id2"));

    if (argc < 3) {
        parser.printHelp();
        binaryFile = "increment.xclbin";
        device_index1 = 1;
        device_index2 = 2;
        //return EXIT_FAILURE;
    }
    for (int i = 0; i < ARRAY_SIZE; i ++)
    {
        test_array1[i] = i;
        test_array2[i] = i;
    }



    std::cout << "Open the device1 : " << device_index1 << std::endl;
    auto device1 = xrt::device(device_index1);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device1.load_xclbin(binaryFile);

    std::cout << "Open the device2 : " << device_index2 << std::endl;
    auto device2 = xrt::device(device_index2);
    auto uuid2 = device2.load_xclbin(binaryFile);

    auto nodma_cnt = 0;
    if (device1.get_info<xrt::info::device::nodma>()) nodma_cnt++;
    if (device2.get_info<xrt::info::device::nodma>()) nodma_cnt++;

    if (nodma_cnt == 2) {
        std::cout
                << "WARNING: P2P transfer can only be done between xdma and nodma devices but not between 2 nodma devices. "
                "Please run this "
                "design on machine with both xdma and nodma devices.\n";
        return 0;
    }

    size_t vector_size_bytes = sizeof(type_t) * ARRAY_SIZE;

    auto krnl1 = xrt::kernel(device1, uuid, "increment");
    auto krnl2 = xrt::kernel(device2, uuid2, "increment");

    xrt::bo::flags flags = xrt::bo::flags::p2p;

#if 0
    std::cout << "Allocate Buffer in Global Memory\n";
    auto in1 = xrt::bo(device1, vector_size_bytes, krnl.group_id(0));
    // If p2p is not enabled, we need to uncomment line 65 and comment line 66.
    // auto out1 = xrt::bo(device1, vector_size_bytes, krnl.group_id(1));
    auto out1 = xrt::bo(device2, vector_size_bytes, flags, krnl2.group_id(2));

    // Map the contents of the buffer object into host memory
    auto in1_map = in1.map<int*>();



    // Create the test data

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        in1_map[i] = i * 2;
        bufReference[i] = in1_map[i] + 10;
    }

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";
    in1.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(in1, out1, 10, ARRAY_SIZE); // data should be in device2
    run.wait();

    auto out1_map = out1.map<int*>();
    auto in2 = xrt::bo(device2, out1_map, vector_size_bytes, 0);

    auto out2 = xrt::bo(device2, vector_size_bytes, krnl2.group_id(1));



    /* gpu internal memory  */
    gpu_copy_benchmark(nullptr);
    /* gpu zero-copy to cpu address */
    gpu_copy_benchmark(test_array);
    /* gpu zero-copy to fpga address (device 2) */
#endif

    gpu_copy_benchmark(test_array1, test_array2);
    auto p2p_in1 = xrt::bo(device1, vector_size_bytes, flags, krnl1.group_id(0));
    auto p2p_in2 = xrt::bo(device1, vector_size_bytes, flags, krnl1.group_id(1));

    auto p2p_in1_map = p2p_in1.map<type_t *>();
    auto p2p_in2_map = p2p_in2.map<type_t *>();

    for (int i = 0; i < ARRAY_SIZE; i ++)
    {
        p2p_in1_map[i] = i;
        p2p_in2_map[i] = i;
    }


    printf("p2p buffer1 address: 0x%lx \n", (unsigned long) (p2p_in1_map)); fflush(stdout);
    printf("p2p buffer2 address: 0x%lx \n", (unsigned long) (p2p_in2_map)); fflush(stdout);
    gpu_copy_benchmark(p2p_in1_map, p2p_in2_map);



    {
        xrt::queue main_queue;
        auto dest_in1 = xrt::bo(device2, p2p_in1_map, vector_size_bytes, 0);

        auto dest_in2 = xrt::bo(device2, p2p_in2_map, vector_size_bytes, 0);

        std::cout << "Run host issued fpga p2p benchmark " << std::endl;
        fflush(stdout);

        int loop = LOOP;
        std::cout << "Bytes Transfer = " << vector_size_bytes << std::endl;
        for (int i = 0; i < loop; i++) {
            std::chrono::high_resolution_clock::time_point t_start = std::chrono::high_resolution_clock::now();

            //main_queue.enqueue([&dest_in1] {dest_in1.sync(XCL_BO_SYNC_BO_TO_DEVICE); });
            auto event = main_queue.enqueue([&dest_in2] {dest_in2.sync(XCL_BO_SYNC_BO_TO_DEVICE); });

            event.wait();

            std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
            // Calculations
            unsigned long t_duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();
            double secduration = t_duration / (double)1000000;
            double gbpersec = ((vector_size_bytes) / secduration) / ((double)1024 * 1024 * 1024);
            std::cout << "Iterations = " << i
                      << "\nP2P Throughput= " << std::setprecision(2) << std::fixed << gbpersec << "GB/s\n";
        }

    }



#if 0

    {
        std::cout << "Run host issued fpga p2p benchmark " << std::endl;
        fflush(stdout);

        int loop = LOOP;
        std::cout << "Bytes Transfer = " << vector_size_bytes << std::endl;
        for (int i = 0; i < loop; i++) {
            std::chrono::high_resolution_clock::time_point t_start = std::chrono::high_resolution_clock::now();

            in2.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
            // Calculations
            unsigned long t_duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();
            double secduration = t_duration / (double)1000000;
            double gbpersec = ((vector_size_bytes) / secduration) / ((double)1024 * 1024 * 1024);
            std::cout << "Iterations = " << i
                      << "\nP2P Throughput= " << std::setprecision(2) << std::fixed << gbpersec << "GB/s\n";
        }

    }

    {
        // Get the output;
        std::cout << "Run fpga2 access fpga1" << std::endl;
        fflush(stdout);

        int loop = LOOP;
        std::chrono::high_resolution_clock::time_point t_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < loop; i++) {
            auto run = krnl2(out1, out2, 0, ARRAY_SIZE);
            run.wait();
        }
        std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
        // Calculations
        unsigned long t_duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();
        double secduration = t_duration / (double)1000000;
        double gbpersec = ((loop * vector_size_bytes) / secduration) / ((double)1024 * 1024 * 1024);
        std::cout << "Bytes Transfer = " << vector_size_bytes << " Iterations = " << loop
                  << "\nFPGA2 kernel Throughput= " << std::setprecision(2) << std::fixed << gbpersec << "GB/s\n";

        // Validate our results
        out2.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        auto out2_map = out2.map<int*>();
        for (int i = 0; i < ARRAY_SIZE; ++i) {
            if ( out2_map[i] != SRC_VALUE)
            {
                printf("[%d] expected %d, get %d\n", i, SRC_VALUE, out2_map[i]);
                return -1;
            }
        }

        std::cout << "TEST PASSED\n";
    }
#endif
    return 0;
}
