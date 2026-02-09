cmake_minimum_required(VERSION 3.5)
project(ts1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DOUBLE "Use double instead of float" OFF)

if(DOUBLE)
    add_executable(ts1 double.cpp)
else()
    add_executable(ts1 float.cpp)
endif()
