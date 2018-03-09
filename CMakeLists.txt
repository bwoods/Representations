cmake_minimum_required(VERSION 3.9)
project(rep CXX C)

set(CMAKE_CXX_STANDARD 17) # for all targets
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	message("Defaulting to a Release build…")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()


add_subdirectory(benchmarks)

