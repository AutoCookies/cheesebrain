#!/usr/bin/env bash
#  MIT license
#  Copyright (C) 2024 Intel Corporation
#  SPDX-License-Identifier: MIT

mkdir -p build
cd build
source /opt/intel/oneapi/setvars.sh

#for FP16
#cmake .. -DGGML_SYCL=ON -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx -DGGML_SYCL_F16=ON -DCHEESE_OPENSSL=OFF # faster for long-prompt inference

#for FP32
cmake .. -DGGML_SYCL=ON -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx -DCHEESE_OPENSSL=OFF

#build example/main
#cmake --build . --config Release --target main

#build example/cheese-bench
#cmake --build . --config Release --target cheese-bench

#build all binary
cmake --build . --config Release -j -v
