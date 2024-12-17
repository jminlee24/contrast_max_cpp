cmake_minimum_required(VERSION "3.19.2")


project(main)
set(CMAKE_CXX_STANDARD 17)
add_executable(main "src/main.cpp")
target_include_directories(main PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(main)

