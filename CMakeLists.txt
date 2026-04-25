cmake_minimum_required(VERSION 3.20)
project(rocky)

enable_testing()

if(WIN32)
  set(CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}/Library")
  set(LLVM_USE_SHARED_LIBS ON)
else()
  set(CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}")
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(external/unity/src)
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

#main app
add_executable(rocky src/main.c)
target_include_directories(rocky PUBLIC include)
if(WIN32)
  target_link_libraries(rocky LLVM-C)
else()
  llvm_map_components_to_libnames(llvm_libs core support native target mc asmprinter orcjit executionengine)
  target_link_libraries(rocky ${llvm_libs})
endif()

#testing setup
add_executable(test_main
    tests/core/test_main.c
    external/unity/src/unity.c
)
target_include_directories(test_main PUBLIC include external/unity/src)
if(WIN32)
  target_link_libraries(test_main LLVM-C)
else()
  target_link_libraries(test_main ${llvm_libs})
endif()
add_test(NAME test_main COMMAND test_main)

#testing of math
add_executable(test_math
    tests/core/test_math.c
    src/math.c
    external/unity/src/unity.c
)

target_include_directories(test_math PUBLIC include external/unity/src)

if(WIN32)
  target_link_libraries(test_math LLVM-C)
else()
  target_link_libraries(test_math ${llvm_libs})
endif()

add_test(NAME test_math COMMAND test_math)
