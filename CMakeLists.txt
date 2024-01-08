cmake_minimum_required(VERSION 3.24)
project(ldbc_analyze)

set(CMAKE_CXX_STANDARD 17)

add_executable(ldbc_analyze main.cpp)

add_executable(idle_time idle_time.cc)
add_executable(query_freq_lat query_freq_lat.cc)
