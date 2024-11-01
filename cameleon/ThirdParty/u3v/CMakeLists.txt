cmake_minimum_required(VERSION 3.5)

project(u3v LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    src/u3v.cpp
    src/control_handle.cpp
    src/register_map.cpp
    src/stream_handle.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

