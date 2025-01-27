cmake_minimum_required(VERSION "3.19.2")

project(main)

find_package(Eigen3 REQUIRED )

add_executable(main src/main.cpp)
target_link_libraries(main Eigen3::Eigen)

