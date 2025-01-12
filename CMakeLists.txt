cmake_minimum_required(VERSION 3.29)
project(rgr4)

#set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "C++ compiler" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror  -fsanitize=address -pedantic -Wconversion -Wsign-conversion -fsanitize=undefined -Wmissing-declarations")

add_subdirectory(DArray)
add_subdirectory(Translator1)

set(SOURCES main.cpp)
add_executable(rgr4 ${SOURCES})

target_link_libraries(rgr4 DArray Translator1)