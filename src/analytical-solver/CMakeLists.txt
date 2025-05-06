cmake_minimum_required(VERSION 3.23)

add_subdirectory(petsc-lib)

project(analytical_solver)

add_library(${PROJECT_NAME} STATIC
    AnalyticalSolver.cpp
    PrintingUtils.cpp
    TAOCallbacks.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "."
    FILES
        "AnalyticalSolver.h"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        petsc_lib # TODO: why is this needed to be PUBLIC? (also check all PUBLIC linkages in the project)
    PRIVATE
        callbacks
        utils
)
