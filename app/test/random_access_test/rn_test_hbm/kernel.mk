TARGET = hw
DEVICE = xilinx_u55c_gen3x16_xdma_3_202210_1
FREQ = 400

compile_explore_busrt_size:=1

VPP_FLAGS += -DMEM_BURST_BUFFER_SIZE=${compile_explore_busrt_size}
CPP_FLAGS += -DMEM_BURST_BUFFER_SIZE=${compile_explore_busrt_size}

VPP_FLAGS += -DMEM_TEST_INTERFACE_WIDTH=256
CPP_FLAGS += -DMEM_TEST_INTERFACE_WIDTH=256

CPP_FLAGS += -DNUM_PE=1

include lib/rn_test/kernel.mk