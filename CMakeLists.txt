cmake_minimum_required(VERSION "3.19.2")

project(main)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(main src/main.cpp)

target_link_libraries(main filereader)
add_subdirectory(lib)
