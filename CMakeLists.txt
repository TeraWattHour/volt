cmake_minimum_required(VERSION 3.28)
project(volt C)

set(CMAKE_C_STANDARD 11)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

include_directories(
    "${SRC_DIR}"
    "${SRC_DIR}/ast"
    "${SRC_DIR}/lexer"
    "${SRC_DIR}/parser"
)

file(GLOB_RECURSE C_FILES "${SRC_DIR}/*.c")

add_executable(volt ${C_FILES})