TARGET ?= hw_emu
DEVICE = xilinx_u55c_gen3x16_xdma_3_202210_1
FREQ = 400



include lib/common_host.mk

include host/pcg/Makefile
include host/xcl2/Makefile
include host/helper/Makefile
include host/log/Makefile
include host/xgraph/Makefile


include src/rng_data_type/Makefile

include test/host_nb_rng/Makefile

include src/irsgu/Makefile
include src/lv_sou/Makefile
include test/test_store_32/Makefile