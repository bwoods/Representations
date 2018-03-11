cmake_minimum_required(VERSION 3.9)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-fsanitize=address,undefined" COMPILER_SUPPORTS_SANITIZERS)
if(COMPILER_SUPPORTS_SANITIZERS)
		set(CMAKE_CXX_FLAGS "-fno-omit-frame-pointer -fsanitize=address,undefined ${CMAKE_CXX_FLAGS}")
		set(CMAKE_C_FLAGS "-fno-omit-frame-pointer -fsanitize=address,undefined ${CMAKE_C_FLAGS}")
endif()


add_subdirectory(rapidcheck)
add_executable(vector_tests vector_tests.cpp vector_tests.cpp)
target_include_directories(vector_tests PRIVATE ..)
target_link_libraries(vector_tests rapidcheck)
add_test(vector_tests vector_tests)

