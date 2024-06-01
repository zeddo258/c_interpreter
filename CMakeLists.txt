
cmake_minimum_required(VERSION 3.0)
project(cpp03_json_project)

set(CMAKE_CXX_STANDARD 11)
include_directories(include)


# Add the -g flag for debugging
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(parser parser.cpp)
