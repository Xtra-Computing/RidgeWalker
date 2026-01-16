TARGET ?= hw_emu
DEVICE = xilinx_u55c_gen3x16_xdma_3_202210_1
FREQ = 400



CPP_FLAGS += -DNUM_OF_SWITCH_STREAMS=8

include lib/common_host.mk

include host/pcg/Makefile
include host/xcl2/Makefile
include host/helper/Makefile
include host/log/Makefile
include host/xgraph/Makefile


include src/rng_data_type/Makefile
include src/rw_data_type/Makefile

include src/switch_data_type/Makefile
include src/switch_butterfly/Makefile

include src/vdirect_dc/Makefile
include src/end_ctrl/Makefile


include test/test_switch_dummy_package_single/Makefile
include test/host_vcache_dc/Makefile


