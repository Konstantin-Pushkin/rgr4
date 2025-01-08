cmake_minimum_required(VERSION 3.29)
project(rgr4)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror  -fsanitize=address -pedantic -Wconversion -Wsign-conversion -fsanitize=undefined -Wmissing-declarations")
