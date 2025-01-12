cmake_minimum_required(VERSION 3.29)
project(rgr4)

# set(CMAKE_CXX_COMPILER clang++)
# указать свой к компилятору. используется компилятор, установленный вручную (brew install llvm)
set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang" CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++" CACHE STRING "C++ compiler" FORCE)

if (CMAKE_CXX_COMPILER MATCHES "/opt/homebrew/opt/llvm/bin/clang\\+\\+")
    message(STATUS "Using clang++: ${CMAKE_CXX_COMPILER}")
    message(STATUS "Version clang++: ${CMAKE_CXX_COMPILER_VERSION}")
elseif (CMAKE_CXX_COMPILER MATCHES "/Library/Developer/CommandLineTools/usr/bin/(clang\\+\\+|c\\+\\+)")
    message(STATUS "Using AppleClang ${CMAKE_CXX_COMPILER}")
    message(STATUS "Version AppleClang: ${CMAKE_CXX_COMPILER_VERSION}")
else ()
    message(STATUS "Unknown compiler: ${CMAKE_CXX_COMPILER}")
    message(STATUS "Versio unknown compiler: ${CMAKE_CXX_COMPILER_VERSION}")
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fsanitize=address -pedantic -Wconversion -Wsign-conversion -fsanitize=undefined -Wmissing-declarations")

add_subdirectory(DArray)
add_subdirectory(Translator1)

set(SOURCES main.cpp)
add_executable(rgr4 ${SOURCES})

target_link_libraries(rgr4 DArray Translator1)