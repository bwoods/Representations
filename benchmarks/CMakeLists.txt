cmake_minimum_required(VERSION 3.9)


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif()


add_executable(benchmarks runner.cpp
	../memory/shared.hpp ../memory/bagwell.hpp ../memory/stl.hpp)
target_include_directories(benchmarks PRIVATE .. nonius/include)
enable_testing()
add_test(NAME benchmarks COMMAND benchmarks WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

