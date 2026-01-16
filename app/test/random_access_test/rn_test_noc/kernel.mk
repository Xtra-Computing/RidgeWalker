TARGET = hw
DEVICE = xilinx_vck5000_gen4x8_qdma_2_202220_1
FREQ = 250

__AIE_SET__ = true

compile_explore_busrt_size:=256


VPP_FLAGS += -DMEM_BURST_BUFFER_SIZE=${compile_explore_busrt_size}
CPP_FLAGS += -DMEM_BURST_BUFFER_SIZE=${compile_explore_busrt_size}

VPP_FLAGS += -DMEM_TEST_INTERFACE_WIDTH=512
CPP_FLAGS += -DMEM_TEST_INTERFACE_WIDTH=512

CPP_FLAGS += -DNUM_PE=4

include lib/rn_test/kernel.mk