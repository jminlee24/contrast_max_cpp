cmake_minimum_required(VERSION "3.19.2")
set (CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release E)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  message(WARNING "It is recommended CMAKE_BUILD_TYPE is set to Release for the best performance \(-DCMAKE_BUILD_TYPE=Release\)")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(main)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 QUIET)
find_package(nlohmann_json QUIET)

if(NOT Eigen3_FOUND)
  set(Eigen3_DIR ${CMAKE_SOURCE_DIR}/packages/eigen3.4.0/cmake)
  find_package(Eigen3 REQUIRED)
endif()

if(NOT nlohmann_json_FOUND)
  set(nlohmann_json_DIR ${CMAKE_SOURCE_DIR}/packages/json/build)
  find_package(nlohmann_json QUIET)
endif()

add_executable(main src/main.cpp)

target_link_libraries(main filereader)
target_link_libraries(main contrastmax)

target_link_libraries(main Eigen3::Eigen)
target_link_libraries(main nlohmann_json::nlohmann_json)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/optim/)

add_subdirectory(lib)

