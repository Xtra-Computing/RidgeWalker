#!/bin/bash

APP=rn_test_hbm

DIR=../burst_size_explore_${APP}

mkdir -p ./${DIR}/build_log/
mkdir -p ./${DIR}/exec_log/
mkdir -p ./${DIR}/bitstream/

PARA_LIST=( '1' \
            '2' \
            '4' \
            '8' \
            '16' \
            '32' \
            '64' \
)

cp build_dir_${APP}/kernel.xclbin ${DIR}/kernel.xclbin

#for para_list  in "${PARA_LIST[@]}"
#for para_list in {1..40}

for current_para in "${PARA_LIST[@]}"
do
  script -c "make test=${APP}  compile_explore_busrt_size=${current_para} cleanall"
  script -c "make test=${APP}  compile_explore_busrt_size=${current_para} TARGET=hw all -j   2>&1"   ./${DIR}/build_log/build_${current_para}.log
  cp -r  build_dir_${APP}  ./${DIR}/bitstream/build_dir_${APP}_burst_size_${current_para}

  #./${APP}.app ${DIR}/kernel.xclbin  | tee ./${DIR}/exec_log/exec_${para_list}.log

  echo "next para"
  #wait

done

