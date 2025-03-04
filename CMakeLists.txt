cmake_minimum_required(VERSION 3.14...3.18)

project(cusz_asap CUDA CXX)
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)

## Policy CMP0104 is introduced as of version 3.18 (policy_max).
## check `cmake --help-policy CMP0104` for more detail.
## The maximum possible in compatibility can be set in CMakeLists.txt using
## `set(CMAKE_CUDA_ARCHITECTURES 60 61 62 70 72 75 80 86)`.
## For command line, use, for example `cmake -DCMAKE_CUDA_ARCHITECTURES="75" ..` 
## to specify CUDA arch.

enable_language(CUDA)
find_package(CUDAToolkit REQUIRED)

#include_directories(src)
#include_directories(src/pSZ)

## TODO flag only add to a specific library, e.g. suppressing deprecation on CUDA10 cuSPARSE
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --extended-lambda --expt-relaxed-constexpr -Wno-deprecated-declarations")
set(LIB_TYPE STATIC)

add_library(pq ${LIB_TYPE} src/wrapper/extrap_lorenzo.cu src/wrapper/interp_spline3.cu)

add_library(argp ${LIB_TYPE} src/utils/format.cc src/context.cc)

add_library(sp ${LIB_TYPE} src/wrapper/csr11.cu)
target_link_libraries(sp PUBLIC CUDA::cusparse)

add_library(huff ${LIB_TYPE} src/wrapper/huffman_parbook.cu src/wrapper/huffman_coarse.cu)
target_link_libraries(huff PUBLIC CUDA::cudart CUDA::cusparse)
set_target_properties(huff PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(huff PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON)
set_target_properties(huff PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

add_library(compress ${LIB_TYPE} src/default_path.cu src/base_compressor.cu src/sp_path.cu)

add_library(cusz-lib ${LIB_TYPE} src/query.cc src/app.cu)
target_link_libraries(cusz-lib PUBLIC CUDA::cudart CUDA::cuda_driver)

add_executable(cusz-bin src/cusz-cli.cu)
target_link_libraries(cusz-bin cusz-lib compress argp huff sp pq)
set_target_properties(cusz-bin PROPERTIES OUTPUT_NAME cusz)
