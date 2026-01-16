TARGET ?= hw_emu
DEVICE = xilinx_u55n_gen3x4_xdma_2_202110_1
FREQ = 250


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
include test/end_ctrl_16/Makefile


include src/irsgu/Makefile
include src/lv_sou/Makefile
include src/lv_rpa_router/Makefile
include src/lv_rpa_access/Makefile

include src/lv_sample/Makefile

include src/lv_cla_router/Makefile
include src/lv_cla_access/Makefile

include test/lv_host_urw/Makefile

include src/lv_rpa_ws_scheduler/Makefile

include test/lv_test_rpa_loader/Makefile

