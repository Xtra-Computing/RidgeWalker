TARGET ?= hw
DEVICE = xilinx_u55c_gen3x16_xdma_3_202210_1
FREQ = 220



CPP_FLAGS += -DNUM_OF_SWITCH_STREAMS=8


CPP_FLAGS += -std=c++17
include mk/lib/jsoncpp.mk

include lib/common_host.mk

include host/pcg/Makefile
include host/xcl2/Makefile
include host/helper/Makefile
include host/log/Makefile
include host/xgraph/Makefile


include src/rng_data_type/Makefile
include src/rw_data_type/Makefile
include src/switch_butterfly/Makefile
include src/switch_data_type/Makefile

include host/network/Makefile
include src/network_u55c/Makefile

include src/lv_rpa_ws_scheduler/Makefile
include src/lv_rpa_crouter/Makefile

include test/lv_test_rpa_loader/Makefile


include test/lv_host_cswitch_test_net/Makefile


include src/irsgu/Makefile
include src/lv_sou/Makefile


