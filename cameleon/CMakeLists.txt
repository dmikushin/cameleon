cmake_minimum_required(VERSION 3.5)

project(cameleon LANGUAGES CXX)

add_subdirectory(ThirdParty/u3v)

add_library(${PROJECT_NAME} STATIC
    src/Camera.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} u3v)

add_executable(${PROJECT_NAME}_test src/test/test.cpp)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
