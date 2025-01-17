cmake_minimum_required(VERSION 3.29)
project(rgr4)

if(APPLE)
    message(STATUS "Building on macOS")
    set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang" CACHE STRING "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++" CACHE STRING "C++ compiler" FORCE)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building on Linux (Ubuntu)")
    set(CMAKE_C_COMPILER "/usr/bin/clang" CACHE STRING "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "C++ compiler" FORCE)
else()
    message(FATAL_ERROR "Unsupported operating system")
endif()

if(CMAKE_CXX_COMPILER MATCHES ".*/clang\\+\\+")
    if(APPLE)
        message(STATUS "Using clang++ for macOS: ${CMAKE_CXX_COMPILER}")
    else()
        message(STATUS "Using clang++ for Ubuntu: ${CMAKE_CXX_COMPILER}")
    endif()
    message(STATUS "Version clang++: ${CMAKE_CXX_COMPILER_VERSION}")
elseif(CMAKE_CXX_COMPILER MATCHES ".*/g\\+\\+|.*/c\\+\\+")
    if(APPLE)
        message(STATUS "Using AppleClang: ${CMAKE_CXX_COMPILER}")
    else()
        message(STATUS "Using GCC: ${CMAKE_CXX_COMPILER}")
    endif()
    message(STATUS "Version: ${CMAKE_CXX_COMPILER_VERSION}")
else()
    message(STATUS "Unknown compiler: ${CMAKE_CXX_COMPILER}")
    message(STATUS "Version unknown compiler: ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fsanitize=address -pedantic -Wconversion -Wsign-conversion -fsanitize=undefined -Wmissing-declarations")

add_subdirectory(DArray)
add_subdirectory(Translator1)

set(SOURCES main.cpp)
add_executable(rgr4 ${SOURCES})

target_link_libraries(rgr4 DArray Translator1)