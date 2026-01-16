TARGET = hw
DEVICE = xilinx_u55n_gen3x4_xdma_2_202110_1
FREQ = 400

compile_explore_busrt_size:=1

VPP_FLAGS += -DMEM_BURST_BUFFER_SIZE=${compile_explore_busrt_size}

include lib/rn_test/kernel.mk