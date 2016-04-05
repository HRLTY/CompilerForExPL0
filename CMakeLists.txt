cmake_minimum_required(VERSION 3.1.0)
set (CMAKE_CXX_STANDARD 11)
file(GLOB PL0 *.cpp *.h)
add_executable(pl0 ${PL0})

