cmake_minimum_required(VERSION 3.29)
project(Translator1)

set(SOURCES src/Interpreter.cpp src/LexicalAnalyzer.cpp)
set(HEADERS include/Interpreter.hpp include/LexicalAnalyzer.hpp)

add_library(Translator1 ${SOURCES} ${HEADERS})

target_include_directories(Translator1 PUBLIC include)