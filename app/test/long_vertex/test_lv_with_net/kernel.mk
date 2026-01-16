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
include test/end_ctrl_16/Makefile


include src/lv_rpa_router/Makefile
include src/lv_vdirect_dc/Makefile
#include src/lv_cla_router/Makefile


include test/lv_host_vcache_switch/Makefile
include test/lv_test_dummy_rpa_st/Makefile



include src/network_u55c/Makefile
include test/test_network/Makefile
