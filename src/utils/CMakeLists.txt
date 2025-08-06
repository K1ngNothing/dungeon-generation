cmake_minimum_required(VERSION 3.23)

project(utils)
add_library(${PROJECT_NAME} CLArguments.cpp)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "../"
    FILES
        "../utils/Random.h"
)

add_subdirectory(tests)
