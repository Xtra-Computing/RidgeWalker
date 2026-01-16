TARGET ?= hw_emu
DEVICE = xilinx_u55c_gen3x16_xdma_3_202210_1
FREQ = 300



CPP_FLAGS += -DNUM_OF_SWITCH_STREAMS=4

include lib/common_host.mk

include host/pcg/Makefile
include host/xcl2/Makefile
include host/helper/Makefile
include host/log/Makefile
include host/xgraph/Makefile

include src/rng_data_type/Makefile

include src/switch_data_type/Makefile
include src/switch_butterfly/Makefile
include test/test_butterfly/Makefile
include test/test_butterfly_4x4_cross/Makefile


include test/host_switch_net/Makefile

