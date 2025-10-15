cmake_minimum_required(VERSION 3.10)
project(ParallelQuickSort)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
endif()

add_executable(parallel_quicksort
    src/main.cpp
    src/ParallelQuickSort.cpp
    src/ThreadWrapper.cpp 
)

target_include_directories(parallel_quicksort PRIVATE include)

if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(parallel_quicksort Threads::Threads)
    target_compile_definitions(parallel_quicksort PRIVATE _UNIX)
    message("Настройка для Linux: pthreads")
endif()

if(WIN32)
    target_compile_definitions(parallel_quicksort PRIVATE _WIN32)
    message("Настройка для Windows: WinAPI")
endif()

message("Платформа: ${CMAKE_SYSTEM_NAME}")
message("Компилятор: ${CMAKE_CXX_COMPILER_ID}")
message("Версия C++: ${CMAKE_CXX_STANDARD}")