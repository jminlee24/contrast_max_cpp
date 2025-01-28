cmake_minimum_required(VERSION "3.19.2")

project(main)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(filereader src/filereader.cpp)

add_executable(${PROJECT_NAME} src/main.cpp)
