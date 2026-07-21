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
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
include_directories(SYSTEM external/unity/src)
include_directories(SYSTEM external)
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})

set(ARGPARSE_SOURCES external/argparse/argparse.c)

add_executable(rocky src/main.c src/cli.c src/debug.c src/lexer.c src/parser.c src/jit.c src/symtable.c src/sema.c ${ARGPARSE_SOURCES})
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


#Testing Part
set(FAKE_PARSER_BIN_DIR "${CMAKE_BINARY_DIR}/tools")

add_executable(fake_parser tools/fake_parser.c)
set_target_properties(fake_parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${FAKE_PARSER_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${FAKE_PARSER_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${FAKE_PARSER_BIN_DIR}
)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.c
)

list(REMOVE_ITEM SRC_FILES ${CMAKE_SOURCE_DIR}/src/main.c)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/tests/*.c
)

list(FILTER TEST_SOURCES EXCLUDE REGEX "[\\/]tests[\\/]lit[\\/]")

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_file ${test_src} NAME_WE)

    get_filename_component(test_dir ${test_src} DIRECTORY)
    file(RELATIVE_PATH rel_path_from_tests ${CMAKE_SOURCE_DIR}/tests ${test_dir})

    if(rel_path_from_tests STREQUAL "" OR rel_path_from_tests STREQUAL ".")
        set(test_name "tests_${test_file}")
        set(test_label "${test_file}")
        set(test_rel_dir "tests")
    else()
        string(REPLACE "/" "_" rel_path_slug ${rel_path_from_tests})
        set(test_name "tests_${rel_path_slug}_${test_file}")
        set(test_label "${rel_path_from_tests}/${test_file}")
        set(test_rel_dir "tests/${rel_path_from_tests}")
    endif()

    add_executable(${test_name}
        ${test_src}
        ${SRC_FILES}
        ${ARGPARSE_SOURCES}
        external/unity/src/unity.c
    )
    target_include_directories(${test_name} PUBLIC
        include
    )
    target_include_directories(${test_name} SYSTEM PUBLIC
        external/unity/src
        external/argparse
    )

    if(WIN32)
        target_link_libraries(${test_name} LLVM-C)
    else()
        target_link_libraries(${test_name} ${llvm_libs})
    endif()

    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${test_rel_dir}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${test_rel_dir}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${test_rel_dir}
    )

    add_test(NAME ${test_name} COMMAND ${test_name})

    set_tests_properties(${test_name} PROPERTIES
        LABELS ${test_label}
    )
endforeach()

set(LIT_TEST_ROOT "${CMAKE_SOURCE_DIR}/tests/lit")
find_program(LLVM_LIT NAMES llvm-lit llvm-lit.py lit
    HINTS ${LLVM_TOOLS_BINARY_DIR} ${CMAKE_PREFIX_PATH}/bin
)
find_program(FILECHECK_EXE NAMES FileCheck FileCheck.exe
    HINTS ${LLVM_TOOLS_BINARY_DIR} ${CMAKE_PREFIX_PATH}/bin ${CMAKE_PREFIX_PATH}/libexec/llvm
)

if(EXISTS "${LIT_TEST_ROOT}" AND LLVM_LIT AND FILECHECK_EXE)
    set(FAKE_PARSER_BIN "${FAKE_PARSER_BIN_DIR}/fake_parser${CMAKE_EXECUTABLE_SUFFIX}")

    file(GLOB_RECURSE LIT_TEST_SOURCES CONFIGURE_DEPENDS
        ${LIT_TEST_ROOT}/**/*.rocky
    )

    foreach(lit_src ${LIT_TEST_SOURCES})
        file(RELATIVE_PATH lit_rel_path ${LIT_TEST_ROOT} ${lit_src})
        string(REGEX REPLACE "\\.rocky$" "" lit_label_path "${lit_rel_path}")
        string(REPLACE "/" "_" lit_rel_name "${lit_label_path}")
        set(lit_test_name "lit_${lit_rel_name}")

        add_test(NAME ${lit_test_name}
            COMMAND ${LLVM_LIT}
                -v
                -Dparser=${FAKE_PARSER_BIN}
                -Drocky=$<TARGET_FILE:rocky>
                -Dfilecheck=${FILECHECK_EXE}
                ${lit_src}
        )

        set_tests_properties(${lit_test_name} PROPERTIES
            LABELS "lit/${lit_label_path}"
        )
    endforeach()
elseif(EXISTS "${LIT_TEST_ROOT}")
    message(WARNING "Lit tests are configured but llvm-lit or FileCheck was not found.")
endif()
