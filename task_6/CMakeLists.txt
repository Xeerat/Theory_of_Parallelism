cmake_minimum_required(VERSION 3.10)

project(1 CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_COMPILER pgc++)

set(CMAKE_CXX_FLAGS "-O2 -acc -Minfo=all")

add_executable(app main.cpp laplace2d.cpp)