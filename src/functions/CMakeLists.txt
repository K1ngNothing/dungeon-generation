cmake_minimum_required(VERSION 3.23)

project(functions)
add_library(${PROJECT_NAME} STATIC
    CorridorLength.cpp
    RoomOverlap.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "../"
    FILES
        "../functions/CorridorLength.h"
        "../functions/Defs.h"
        "../functions/RoomOverlap.h"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        model
)
