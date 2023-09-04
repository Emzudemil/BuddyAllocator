cmake_minimum_required(VERSION 3.26)
project(buddyalloc C)

set(CMAKE_C_STANDARD 11)

add_executable(buddyalloc main.c
        BuddyAllocator.c
        Math.c
        Math.h)
