cmake_minimum_required(VERSION 3.16.3)
project(mumu)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(mumu)

find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)

set(LIB_SRC
    mumu/log.cc
    mumu/util.cc
    mumu/config.cc
    mumu/thread.cc
    )

add_library(mumu SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(mumu)

set(LIB_LIB
    mumu
    pthread
    yaml-cpp
    )

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config mumu)
force_redefine_file_macro_for_sources(test_config)
target_link_libraries(test_config ${LIB_LIB})

add_executable(test tests/test.cc)
add_dependencies(test mumu)
force_redefine_file_macro_for_sources(test)
target_link_libraries(test ${LIB_LIB})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread mumu)
force_redefine_file_macro_for_sources(test_thread)
target_link_libraries(test_thread ${LIB_LIB})

add_executable(test_util tests/test_util.cc)
add_dependencies(test_util mumu)
force_redefine_file_macro_for_sources(test_util)
target_link_libraries(test_util ${LIB_LIB})


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
