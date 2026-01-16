TARGET ?= hw_emu
DEVICE = xilinx_u55c_gen3x16_xdma_3_202210_1
FREQ = 300


CPP_FLAGS += -DHW_VCACHE

ifeq "${TARGET}" "hw_emu"

EMU_ARGS+= -qsize 512


endif


include lib/common_host.mk

include src/rng_data_type/Makefile
include src/rw_data_type/Makefile


include host/log/Makefile
include host/pcg/Makefile
include host/xcl2/Makefile
include host/helper/Makefile
include host/xgraph/Makefile
include host/urw/Makefile


include src/irsgu/Makefile
include src/sou/Makefile
include src/ring_manager/Makefile
include src/step_loader/Makefile
include src/urw/Makefile

include test/step_counter/Makefile

include src/vdac_cache/Makefile
include src/cache_direct_update/Makefile