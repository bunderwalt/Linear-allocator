cmake_minimum_required(VERSION 3.31)
project(MyAllocator C)

set(CMAKE_C_STANDARD 11)

add_executable(MyAllocator main.c
        linear_allocator.h
        linear_allocator.c
)
