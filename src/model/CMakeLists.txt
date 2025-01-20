cmake_minimum_required(VERSION 3.23)

project(model)
add_library(${PROJECT_NAME} STATIC
    Door.cpp
    Room.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "../"
    FILES
        "../model/Door.h"
        "../model/Model.h"
        "../model/Room.h"
        "../model/Variables.h"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        analytical-solver
)
