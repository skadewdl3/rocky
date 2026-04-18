cmake_minimum_required(VERSION 3.20)
project(rocky)

set(CMAKE_PREFIX_PATH $ENV{CONDA_PREFIX})
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

add_executable(rocky src/test.c)

llvm_map_components_to_libnames(llvm_libs core support native target mc asmprinter orcjit executionengine)
target_link_libraries(rocky ${llvm_libs})
