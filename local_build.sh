#!/bin/bash

module="UESTC_VHM"
log_dir="$HOME/uestc_vhm_log"

mkdir -p "$log_dir"
if [[ ! -d "$log_dir" ]]; then
    echo "create log dir failed"
    exit 1
fi

log_path="$log_dir/uestc_vhm_log_$(date +'%Y_%m_%d_%H_%M_%S').log"

log_save_without_time() {
    echo "$1" "$2" >> "$log_path"
}

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
start_time=$(date +'%s')

# cmake
cmake -S . -B build \
-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
-DBUILD_PLATFORM=x86_64 \
-DCMAKE_BUILD_TYPE=Release \
> "/tmp/uestc_vhm_build.log" 2>&1
ret=$?
log_save_without_time "$(cat "/tmp/uestc_vhm_build.log")"
if [[ $ret -ne 0 ]]; then
    log_error "cmake failed, ret=$ret"
    exit 1
fi
log_info "cmake end"

# format
cmake --build build \
--target format \
-- -j $(nproc) \
> "/tmp/uestc_vhm_build.log" 2>&1
ret=$?
log_save_without_time "$(cat "/tmp/uestc_vhm_build.log")"
if [[ $ret -ne 0 ]]; then
    log_error "format failed, ret=$ret"
    exit 1
fi
log_info "format end"

# cppcheck
cmake --build build \
--target cppcheck \
-- -j $(nproc) \
> "/tmp/uestc_vhm_build.log" 2>&1
ret=$?
log_save_without_time "$(cat "/tmp/uestc_vhm_build.log")"
if [[ $ret -ne 0 ]]; then
    log_error "cppcheck failed, ret=$ret"
    exit 1
fi
log_info "cppcheck end"

# # clang-tidy
# cmake --build build \
# --target tidy \
# -- -j $(nproc) \
# > "/tmp/uestc_vhm_build.log" 2>&1
# ret=$?
# log_save_without_time "$(cat "/tmp/uestc_vhm_build.log")"
# if [[ $ret -ne 0 ]]; then
#     log_error "tidy failed, ret=$ret"
#     exit 1
# fi
# log_info "tidy end"

# make
make -C build -j $(nproc) \
> "/tmp/uestc_vhm_build.log" 2>&1
ret=$?
log_save_without_time "$(cat "/tmp/uestc_vhm_build.log")"
if [[ $ret -ne 0 ]]; then
    log_error "make failed, ret=$ret"
    exit 1
fi
log_info "make end"

end_time=$(date +'%s')
wasted_time=$(( end_time - start_time ))
log_info "build end, wasted time: $wasted_time seconds"
log_info "check the log file($log_path) for build details"

# coredump_gen_dir=$(cat /proc/sys/kernel/core_pattern)
# coredump_gen_dir=$(dirname $coredump_gen_dir)
# # build_file="test_$1"
# # build_path="test"
# build_file="uestc_vhm"
# build_path="/home/xy/work/tensorrt-alpha/uestc_vhm/build/src"
# debug_coredump_dir="../coredump"
# rm -rf $debug_coredump_dir/*

# if [[ -f "$build_path/$build_file" ]]; then
#     $build_path/$build_file --config=/home/xy/work/tensorrt-alpha/uestc_vhm/src/etc/uestc_vhm_cfg.json > "./run.log" 2>&1
#     ret=$?
#     log_save_without_time "$(cat "./run.log")"
#     if [[ $ret -ne 0 ]]; then
#         coredump_file=$(ls -l $coredump_gen_dir | grep $build_file | tail -n1 | awk '{print $9}')
#         echo $coredump_file
#         if [[ "$coredump_file" != "" ]]; then
#             cp $coredump_gen_dir/$coredump_file $debug_coredump_dir
#             cp $build_path/$build_file $debug_coredump_dir
#             # gdb $debug_coredump_dir/$build_file $debug_coredump_dir/$coredump_file
#             log_error "runtime error, please check the coredump directory"
#         fi
#     fi
# fi