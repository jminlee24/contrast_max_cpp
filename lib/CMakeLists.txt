add_library(filereader filereader.cpp)
add_library(contrastmax contrastmax.cpp)

target_include_directories(filereader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(contrastmax PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(filereader Eigen3::Eigen)
target_link_libraries(contrastmax Eigen3::Eigen)
