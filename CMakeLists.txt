cmake_minimum_required(VERSION "3.19.2")
set (CMAKE_CXX_STANDARD 11)

option(PACKAGE_PATH "path to packages (optional)" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  message(WARNING "It is recommended CMAKE_BUILD_TYPE is set to Release for the best performance \(-DCMAKE_BUILD_TYPE=Release\)")
endif()

if(PACKAGE_PATH)
  get_filename_component(PACKAGE_PATH ${PACKAGE_PATH} REALPATH)
  message("PACKAGE_PATH SET: ${PACKAGE_PATH}")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(main)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 QUIET)
find_package(nlohmann_json QUIET)

if(NOT Eigen3_FOUND AND PACKAGE_PATH)
  set(Eigen3_DIR ${PACKAGE_PATH}/eigen-3.4.0/cmake)
  find_package(Eigen3 REQUIRED )
elseif(NOT Eigen3_FOUND)
  set(Eigen3_DIR ${CMAKE_SOURCE_DIR}/packages/eigen-3.4.0/build)
  find_package(Eigen3 QUIET)
endif()

if(NOT nlohmann_json_FOUND AND PACKAGE_PATH)
  set(nlohmann_json_DIR ${PACKAGE_PATH}/json/build)
  find_package(nlohmann_json REQUIRED)
elseif(NOT nlohmann_json_FOUND)
  set(nlohmann_json_DIR ${CMAKE_SOURCE_DIR}/packages/json/build)
  find_package(nlohmann_json QUIET)
endif()

if(NOT nlohmann_json_FOUND)
  message(FATAL_ERROR "Unable to find nlohmann_json in the default paths, please install the packages or set the PACKAGE_PATH option with: \n -DPACKAGE_PATH=\{path\}")
endif()
if(NOT Eigen3_FOUND)
  message(FATAL_ERROR "Unable to find Eigen3 in the default paths, please install the packages or set the PACKAGE_PATH option with: \n -DPACKAGE_PATH=\{path\}")
endif()

add_executable(main src/main.cpp)

target_link_libraries(main filereader)
target_link_libraries(main contrastmax)

target_link_libraries(main Eigen3::Eigen)
target_link_libraries(main nlohmann_json::nlohmann_json)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/optim/)

add_subdirectory(lib)

SET(PACKAGE_PATH OFF CACHE INTERNAL "" FORCE)
