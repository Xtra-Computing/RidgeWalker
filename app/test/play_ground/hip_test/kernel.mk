TARGET ?= hw_emu
DEVICE = xilinx_u250_gen3x16_xdma_3_1_202020_1
FREQ = 300

CPP_FLAGS += -std=c++17

include mk/lib/hip.mk


include host/xcl2/Makefile
include host/helper/Makefile
include host/log/Makefile
include lib/common_host.mk
include host/hip_test/Makefile