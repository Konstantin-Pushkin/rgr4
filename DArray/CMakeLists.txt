cmake_minimum_required(VERSION 3.29)
project(DArray)

set(SOURCES src/DArray.cpp)
set(HEADERS include/DArray.hpp)

add_library(DArray ${SOURCES} ${HEADERS})

target_include_directories(DArray PUBLIC include)