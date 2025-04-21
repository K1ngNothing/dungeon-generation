cmake_minimum_required(VERSION 3.23)

project(random)
add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "../"
    FILES
        "../random/Random.h"
)
