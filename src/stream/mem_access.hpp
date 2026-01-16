#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#include "rng_type.h"

#include <ap_int.h>

/*
 * U250  300
 *  1 87.613046 M  87
 *  2 50.074331 M 100
 *  4 30.237670 M 120
 *  8 20.526755 M 160
 * 16 13.010123 M 208
 * 32 7.360042  M 235
 */

/*
 * HBM
 *  1 142.500877 M  143
 *  2  70.290325 M  140
 *  4  46.299818 M  184
 *  8  24.315100 M  192
 * 16  12.351180 M  198
 * 32  *********
 */

#ifndef MEM_BURST_BUFFER_SIZE
#error  "burst size not specified"
#endif

#ifndef MEM_INTERFACE_WIDTH
#error  "interface width not specified"
#endif

#ifndef MEM_KERNEL_NAME
#error  "kernel name not specified"
#endif

#ifndef MEM_DUMMY_DATA
#error "dummy data is not defined"
#endif


#ifdef  MEM_CTRL_INTERFACE
#warning "ctrl interface is enabled"
#define __EXTERNAL_EXIT_CTRL__  (1)
#else
#define __EXTERNAL_EXIT_CTRL__  (0)
#endif


#define MEM_FLUSH_WINDOW                (16)



const int burst_size = (MEM_BURST_BUFFER_SIZE < 2) ? (2) : (MEM_BURST_BUFFER_SIZE);

const int burst_ii = MEM_BURST_BUFFER_SIZE;

extern "C" {
    void MEM_KERNEL_NAME(
        const ap_uint<MEM_INTERFACE_WIDTH>          *data,
        mem_access_cmd_stream_t                     &in_cmd,
#if __EXTERNAL_EXIT_CTRL__
        ctrl_stream_t                               &in_ctrl,
#endif
        mem_access_stream_t(MEM_INTERFACE_WIDTH)    &out_data
    )
    {

#pragma HLS INTERFACE m_axi port = data offset = slave num_read_outstanding=64  max_read_burst_length=burst_size latency=118

#pragma HLS INTERFACE s_axilite port = data bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

        mem_access_item_t(MEM_INTERFACE_WIDTH) buffer[MEM_BURST_BUFFER_SIZE];
#pragma HLS ARRAY_PARTITION variable=buffer   dim=0

read:       while (1) {
#pragma HLS PIPELINE II=burst_ii


#if __EXTERNAL_EXIT_CTRL__
            ap_uint<1> in_ctrl_empty = in_ctrl.empty();
#else
            ap_uint<1> in_ctrl_empty = 1;
#endif
            ap_uint<2> code;
            ap_uint<1> in_cmd_empty = in_cmd.empty();
            code.range(1, 1) = in_cmd_empty;
            code.range(0, 0) = in_ctrl_empty;
            switch (code)
            {
            case 0b11: break;
            case 0b01:
            case 0b00:
            {
                mem_access_cmd_pkg_t mem_cmd;
                ap_uint<1> last_flag;
                ap_uint<1> dummy_flag;
                mem_cmd = in_cmd.read();


                uint32_t addr = mem_cmd.data.ap_member(mem_access_cmd_t, addr);
                last_flag  =  ((mem_cmd.data.ap_member(mem_access_cmd_t, state) & MM_STATE_MASK_END)   == MM_STATE_MASK_END);
                dummy_flag =  ((mem_cmd.data.ap_member(mem_access_cmd_t, state) & MM_STATE_MASK_DUMMY) == MM_STATE_MASK_DUMMY);
burst:          for (int bi = 0; bi < MEM_BURST_BUFFER_SIZE; bi ++)
                {
                    if (dummy_flag == FLAG_SET) {
                        buffer[bi] = MEM_DUMMY_DATA;
                    }
                    else {
                        buffer[bi] = data[addr + bi];
                    }

                    mem_access_pkg_t(MEM_INTERFACE_WIDTH) data_pkg;
                    data_pkg.data.ap_mem_data(MEM_INTERFACE_WIDTH) = buffer[bi];
                    data_pkg.data.range(MEM_INTERFACE_WIDTH        , MEM_INTERFACE_WIDTH) = dummy_flag;
                    data_pkg.data.range(MEM_INTERFACE_WIDTH + 8 - 1, MEM_INTERFACE_WIDTH + 1) = 0;
                    data_pkg.last =
                        (bi == (MEM_BURST_BUFFER_SIZE - 1)) &&
                        ((mem_cmd.data.ap_member(mem_access_cmd_t, state) & MM_STATE_MASK_END) == MM_STATE_MASK_END);

                    out_data.write(data_pkg);
                }
#if  __EXTERNAL_EXIT_CTRL__
                // Do not exit when this kernel is controlled by external ctrl commands.
#else
                switch (last_flag)
                {
                case 1: return;
                }
#endif
                break;
            }
            case 0b10:
            {
#if __EXTERNAL_EXIT_CTRL__
                ctrl_pkg_t end_pkg;
                end_pkg = in_ctrl.read();
                if ((end_pkg.data  & CTRL_END_OF_STREAM) ==  CTRL_END_OF_STREAM)
                {
                    return;
                }
#endif
                break;
            }
            }

        }
    }
}

#undef  MEM_BURST_BUFFER_SIZE
#undef  MEM_INTERFACE_WIDTH
#undef  MEM_KERNEL_NAME
#undef  MEM_DUMMY_DATA

#ifdef  MEM_CTRL_INTERFACE
#undef  MEM_CTRL_INTERFACE
#undef  __EXTERNAL_EXIT_CTRL__
#endif


