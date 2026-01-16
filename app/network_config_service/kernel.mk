TARGET ?= hw
DEVICE = xilinx_u55c_gen3x16_xdma_3_202210_1
FREQ = 300


CPP_FLAGS += -std=c++17


include mk/lib/jsoncpp.mk
include mk/lib/xrt.mk


include lib/common_host.mk

include src/rng_data_type/Makefile
include src/rw_data_type/Makefile


include host/log/Makefile
include host/xgraph/Makefile

include host/pcg/Makefile
include host/xcl2/Makefile
include host/helper/Makefile

include src/network_u55c/Makefile
include host/network/Makefile

include test/test_network/Makefile



include host/network_config_service/Makefile

NET_CONFIG ?= hacc_gpu2_test

CPP_FLAGS+= -D__USER_NET_CONFIG__=${NET_CONFIG}
