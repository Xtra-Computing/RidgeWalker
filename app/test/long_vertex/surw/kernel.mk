TARGET ?= hw_emu
DEVICE = xilinx_u55c_gen3x16_xdma_3_202210_1
FREQ = 320


VPP_FLAGS += --advanced.param compiler.normalizeDefaultMemoryBanks=true
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

include src/end_ctrl/Makefile

include src/irsgu/Makefile
include src/lv_sou_single/Makefile

include src/lv_rpa_access_single/Makefile

include src/lv_sample/Makefile

include src/lv_cla_access_single/Makefile


include test/lv_single_urw/Makefile

include test/lv_test_single_rpa_rpa_mem/Makefile

