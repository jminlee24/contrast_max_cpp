cmake_minimum_required(VERSION "3.19.2")
set (CMAKE_CXX_STANDARD 11)

project(main)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 REQUIRED)

add_executable(main src/main.cpp)

target_link_libraries(main filereader)
target_link_libraries(main contrastmax)

target_link_libraries(main Eigen3::Eigen)

add_subdirectory(lib)
