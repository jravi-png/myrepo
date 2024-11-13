cmake_minimum_required(VERSION 3.0)
project(RedundantCastOptimizer)

add_executable(optimizer main.cpp)
target_link_libraries(optimizer clangTooling clangASTMatchers clangBasic)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})