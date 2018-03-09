cmake_minimum_required(VERSION 3.9)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
endif()


#CHECK_CXX_COMPILER_FLAG("-msse4.1" COMPILER_SUPPORTS_SSE4_1)
#if(COMPILER_SUPPORTS_SSE4_1)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1")
#endif()


add_executable(benchmarks runner.cpp
	../memory/shared.hpp ../memory/bagwell.hpp ../memory/stl.hpp)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED OFF)
find_package(Boost REQUIRED chrono system)

target_link_libraries(benchmarks PRIVATE ${Boost_LIBRARIES})
target_include_directories(benchmarks PRIVATE .. nonius/include/ ${Boost_INCLUDE_DIRS})

