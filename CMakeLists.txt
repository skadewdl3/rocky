cmake_minimum_required(VERSION 3.20)
project(rocky)

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
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

add_executable(rocky src/main.c src/jit.c)
target_include_directories(rocky PUBLIC include)

if(WIN32)
  target_link_libraries(rocky LLVM-C)

  add_custom_command(TARGET rocky POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "$ENV{CONDA_PREFIX}/Library/bin/LLVM-C.dll"
          "$ENV{CONDA_PREFIX}/Library/bin/zlib.dll"
          "$ENV{CONDA_PREFIX}/Library/bin/zstd.dll"
          $<TARGET_FILE_DIR:rocky>
  )

else()
  llvm_map_components_to_libnames(llvm_libs core support native target mc asmprinter orcjit executionengine)
  target_link_libraries(rocky ${llvm_libs})
endif()
