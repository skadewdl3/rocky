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


#Testing Part
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.c
)

list(REMOVE_ITEM SRC_FILES ${CMAKE_SOURCE_DIR}/src/main.c)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/tests/**/*.c
)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_file ${test_src} NAME_WE)

    get_filename_component(test_dir ${test_src} DIRECTORY)
    file(RELATIVE_PATH rel_dir ${CMAKE_SOURCE_DIR} ${test_dir})

    string(REPLACE "/" "_" rel_dir_name ${rel_dir})
    set(test_name "${rel_dir_name}_${test_file}")

    add_executable(${test_name}
        ${test_src}
        ${SRC_FILES}
        external/unity/src/unity.c
    )

    target_include_directories(${test_name} PUBLIC
        include
        external/unity/src
    )

    if(WIN32)
        target_link_libraries(${test_name} LLVM-C)
    else()
        target_link_libraries(${test_name} ${llvm_libs})
    endif()

    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${rel_dir}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${rel_dir}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${rel_dir}
    )

    add_test(NAME ${test_name} COMMAND ${test_name})

    string(REPLACE "tests/" "" rel_dir_clean ${rel_dir})

    set(test_label "${rel_dir_clean}/${test_file}")

    set_tests_properties(${test_name} PROPERTIES
        LABELS ${test_label}
    )

endforeach()