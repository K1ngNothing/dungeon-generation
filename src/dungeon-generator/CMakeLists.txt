cmake_minimum_required(VERSION 3.23)

project(dungeon_generator)
add_library(${PROJECT_NAME} STATIC
    DungeonGenerator.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "."
    FILES
        "DungeonGenerator.h"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        model_generator
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -DPATH_TO_SVG="${CMAKE_BINARY_DIR}"
)
