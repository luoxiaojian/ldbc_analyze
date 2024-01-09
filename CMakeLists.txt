cmake_minimum_required(VERSION 3.16)
project(ldbc_analyze)

set(CMAKE_CXX_STANDARD 17)
set(DEFAULT_BUILD_TYPE "Release")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g")

add_executable(ldbc_analyze main.cpp)

add_executable(idle_time idle_time.cc)
add_executable(query_freq_lat query_freq_lat.cc)
add_executable(query_percentage query_percentage.cc)
