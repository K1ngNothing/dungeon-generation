cmake_minimum_required(VERSION 3.23)

project(callbacks)
add_library(${PROJECT_NAME} STATIC
    CorridorLength.cpp
    RoomOverlap.cpp
    RoomShaker.cpp
    SVGDumper.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "../"
    FILES
        "../callbacks/CorridorLength.h"
        "../callbacks/Defs.h"
        "../callbacks/RoomOverlap.h"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        model
)
