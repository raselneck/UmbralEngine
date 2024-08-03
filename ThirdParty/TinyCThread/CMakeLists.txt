# TinyCThread files currently based on commit 6957fc8383d6c7db25b60b8c849b29caab1caaee
# https://github.com/tinycthread/tinycthread/tree/6957fc8383d6c7db25b60b8c849b29caab1caaee

cmake_minimum_required(VERSION 3.23)

project(TinyCThread LANGUAGES C VERSION 1.2)

enable_testing()

find_package(Threads REQUIRED)

add_library(tinycthread STATIC Source/tinycthread.c)
target_include_directories(tinycthread PUBLIC "Include")
target_link_libraries(tinycthread ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET tinycthread PROPERTY POSITION_INDEPENDENT_CODE ON)