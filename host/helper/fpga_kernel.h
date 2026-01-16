#ifndef __FPGA_KERNEL_H__
#define __FPGA_KERNEL_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <vector>

#include "xcl2.hpp"
#include "helper.h"

#include <pthread.h>






class cl_accelerator
{
private:
    cl::Device device;
    cl::Context context;
    cl::Program program;
    cl_int err;
public:

    cl_accelerator() {}

    cl_accelerator(int argc, char *argv[])
    {
        std::string binaryFile;
        if (argc < 2) {
            std::string app_name = argv[0];
            app_name = app_name.substr(2, app_name.size() - 6);
            binaryFile = "build_dir_" + app_name + "/kernel.xclbin";
            printf("try to use default xclbin: %s\n", binaryFile.c_str());
        }
        else { binaryFile = argv[1]; }
        printf("starting \n");
        fflush(stdout);
        init_platform(binaryFile);


    }

    cl_accelerator(int argc, char *argv[], int device_id)
    {
        std::string binaryFile;
        if (argc < 2) {
            std::string app_name = argv[0];
            app_name = app_name.substr(2, app_name.size() - 6);
            binaryFile = "build_dir_" + app_name + "/kernel.xclbin";
            printf("try to use default xclbin: %s\n", binaryFile.c_str());
        }
        else { binaryFile = argv[1]; }
        printf("starting \n");
        fflush(stdout);
        init_platform(binaryFile, device_id);

    }


    cl_accelerator(std::string binaryFile)
    {
        init_platform(binaryFile);
    }

    cl_accelerator(std::string binaryFile, int device_id)
    {
        init_platform(binaryFile, device_id);
    }


    int init_platform(std::string binaryFile, int device_id)
    {
        std::vector<cl::Device> devices = xcl::get_xil_devices();
        auto fileBuf = xcl::read_binary_file(binaryFile);
        cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
        bool valid_device = false;
        if (device_id >= devices.size()) {
            printf("invalid device id %d, available devices:%ld\n", device_id, devices.size());
            exit(-1);
        }

        auto local_device = devices[device_id];
        OCL_CHECK(err, context = cl::Context(local_device, nullptr, nullptr, nullptr, &err));
        std::cout << "Trying to program device[" << device_id << "]: " << local_device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program local_program(context, {local_device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << device_id << "] with xclbin file\n";
        } else {
            std::cout << "Device[" << device_id << "]: program successful!\n";
            valid_device = true;
            program = local_program;
            device = local_device;
        }

        if (!valid_device) {
            std::cout << "Failed to program\n";
            return -1;
        }
        return 0;
    }

    int init_platform(std::string binaryFile)
    {
        std::vector<cl::Device> devices = xcl::get_xil_devices();

        for (unsigned int i = 0; i < devices.size(); i++)
        {
            int res = init_platform(binaryFile, i);
            if (res == 0)
                return 0;
        }
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    cl::CommandQueue alloc_task_queue(void)
    {
        cl::CommandQueue q;
        OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                            CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
                                            , &err));
        return q;
    }
    cl::Program get_program(void)
    {
        return program;
    }
    cl::Context get_context(void)
    {
        return context;
    }
    cl::Device get_device(void)
    {
        return device;
    }

};

#define MAX_KERNEL_STRING_LENGTH (1024)

class cl_krnl
{
protected:
    cl_int err;
    cl_mem_ext_ptr_t host_buffer_ext;

    cl::Context context;
    double start, stop;
    bool init_flag = false;

    pthread_t inner_thread;

    void kernel_init(cl_accelerator &acc)
    {
        host_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
        host_buffer_ext.obj = NULL;
        host_buffer_ext.param = 0;

        OCL_CHECK(err, krnl = cl::Kernel(acc.get_program(), kernel_string, &err));

        OCL_CHECK(err, mem_q = cl::CommandQueue(acc.get_context(), acc.get_device(),
                                                CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
                                                , &err));
        context = acc.get_context();
    }

    void * internal_thread(void * p_this)
    {
        cl_krnl* p = static_cast<cl_krnl*>(p_this);
        DEBUG_PRINTF("%s start", p->kernel_string);
        p->mem_q.enqueueTask(((cl_krnl *)p_this)->krnl);
        p->mem_q.finish();
        DEBUG_PRINTF("%s end", p->kernel_string);
        return NULL;
    }


public:
    cl::CommandQueue mem_q;
    cl::Kernel krnl;
    char kernel_string[MAX_KERNEL_STRING_LENGTH];

    cl_krnl() {}

    cl_krnl(cl_accelerator &acc, std::string name, uint32_t id)
    {
        int ns =  snprintf(kernel_string, ARRAY_SIZE(kernel_string), "%s:{%s_%d}",
                           name.c_str(),
                           name.c_str(),
                           id + 1);
        if (ns >= MAX_KERNEL_STRING_LENGTH)
        { DEBUG_ERROR("Overflow error in add kernel: %s", kernel_string); exit(-1); }
        kernel_init(acc);
    }

    //with category id
    cl_krnl(cl_accelerator &acc, std::string name, uint32_t cid, uint32_t id)
    {
        int ns =  snprintf(kernel_string, ARRAY_SIZE(kernel_string), "%s_%d:{%s_%d_%d}",
                           name.c_str(),
                           cid,
                           name.c_str(),
                           cid,
                           id + 1);
        if (ns >= MAX_KERNEL_STRING_LENGTH)
        { DEBUG_ERROR("Overflow error in add kernel: %s", kernel_string); exit(-1); }
        kernel_init(acc);
    }

    cl_krnl(cl_accelerator &acc, std::string name, uint32_t gid, uint32_t id,  uint32_t grid)
    {
        int ns =  snprintf(kernel_string, ARRAY_SIZE(kernel_string), "%s:{%s_%d_%d}",
                           name.c_str(),
                           name.c_str(),
                           gid,
                           id + 1);
        if (ns >= MAX_KERNEL_STRING_LENGTH)
        { DEBUG_ERROR("Overflow error in add kernel: %s", kernel_string); exit(-1); }
        kernel_init(acc);
    }

    void set(int arg_id, cl::Buffer  &buffer)
    {
        OCL_CHECK(err, err = krnl.setArg(arg_id, buffer));
    }
    void async_schedule_task(void)
    {
        pthread_create(&inner_thread, NULL, internal_thread_entry, this);
        //pthread_join(inner_thread, NULL);
        //pthread_detach(inner_thread);
    }
    void async_schedule_task(cl::CommandQueue & q)
    {
        async_schedule_task();
    }
    void async_join(void)
    {
        //pthread_create(&inner_thread, NULL, internal_thread_entry, this);
        pthread_join(inner_thread, NULL);
        //pthread_detach(inner_thread);
    }


    int schedule_task(cl::CommandQueue & q)
    {
        if (init_flag)
        {
            OCL_CHECK(err, err = q.enqueueTask(krnl));
        }
        else
        {
            DEBUG_ERROR("Uninitialization error in kernel: %s", kernel_string); exit(-1);
        }
        return 0;
    }

    int exec(void)
    {
        DEBUG_ERROR("can not exec alone\n")
        return 0;
    }

    int get_res(void)
    {
        DEBUG_ERROR("not implemented\n")
        return 0;
    }

    bool set_init_done(void)
    {
        //only inited kernel can be scheduled to task queue
        init_flag = true;
        return init_flag;
    }
    static void * internal_thread_entry(void * p_this) {((cl_krnl *)p_this)->internal_thread(p_this); return NULL;}


};





#endif /* __FPGA_KERNEL_H__ */
