#!/bin/bash

module="UESTC_VHM"
log_dir="$HOME/uestc_vhm_log"

mkdir -p "$log_dir"
if [[ ! -d "$log_dir" ]]; then
    echo "create log dir failed"
    exit 1
fi

log_path="$log_dir/uestc_vhm_log_$(date +'%Y_%m_%d_%H_%M_%S').log"

log_save() {
    current_time=$(date +'%Y-%m-%d %H:%M:%S')
    echo "[$current_time]$1" "$2" >> "$log_path"
}

log_print() {
    current_time=$(date +'%Y-%m-%d %H:%M:%S')
    echo "[$current_time]$1" "$2"
}

log_info() {
    log_print "[$module][INFO]" "$1"
    log_save "[$module][INFO]" "$1"
}

log_error() {
    log_print "[$module][ERROR]" "$1"
    log_save "[$module][ERROR]" "$1"
    log_print "[$module][ERROR]" "check the log file($log_path) for error details"
}

log_info "build begin"

if [[ -d "build" ]]; then
    rm -rf build
fi

mkdir -p build
cd build

# cmake -DTEST_FILE_NAME=test_${1} -DCMAKE_C_COMPILER="/usr/bin/clang" -DCMAKE_CXX_COMPILER="/usr/bin/clang++" .. 
# --debug-trycompile可以用于查看check_cxx_symbol_exists的查找情况
# cmake -DTEST_FILE_NAME=test_${1} --debug-trycompile .. 
# cmake -DTEST_FILE_NAME=test_${1} .. 
cmake -DBUILD_PLATFORM=x86_64 .. 
ret=$?
if [[ $ret -ne 0 ]]; then
    log_error "cmake failed, ret=$ret"
    exit 1
fi

make -j20 > "./make.log" 2>&1
ret=$?
log_save "$(cat "./make.log")"
if [[ $ret -ne 0 ]]; then
    log_error "make failed, ret=$ret"
    exit 1
fi

log_info "build end"
log_info "check the log file($log_path) for build details"

coredump_gen_dir=$(cat /proc/sys/kernel/core_pattern)
coredump_gen_dir=$(dirname $coredump_gen_dir)
# build_file="test_$1"
# build_path="test"
build_file="src/uestc_vhm"
build_path="/home/xy/work/tensorrt-alpha/uestc_vhm/build"
debug_coredump_dir="../coredump"
rm -rf $debug_coredump_dir/*

if [[ -f "$build_path/$build_file" ]]; then
    $build_path/$build_file --model=/home/xy/work/tensorrt-alpha/data/yolov8/yolov8n.trt \
    --batch=a \
    --config=/home/xy/work/tensorrt-alpha/uestc_vhm/src/config/uestc_vhm_cfg.json \
    --save=/tmp/uestc_vhm_save_dir \
    --show
    if [[ $? -ne 0 ]]; then
        log_error "runtime error"
        coredump_file=$(ls -l $coredump_gen_dir | grep $build_file | tail -n1 | awk '{print $9}')
        echo $coredump_file
        if [[ "$coredump_file" != "" ]]; then
            cp $coredump_gen_dir/$coredump_file $debug_coredump_dir
            cp $build_path/$build_file $debug_coredump_dir
            # gdb $debug_coredump_dir/$build_file $debug_coredump_dir/$coredump_file
            log_error "runtime error, please check the coredump directory"
        fi
    fi
fi