cmake_minimum_required(VERSION 3.23)

project(model_generator)
add_library(${PROJECT_NAME} STATIC
    ModelGenerator.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "."
    FILES
        "ModelGenerator.h"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        model
)
