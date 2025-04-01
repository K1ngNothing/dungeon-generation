cmake_minimum_required(VERSION 3.23)

project(model)
add_library(${PROJECT_NAME} STATIC
    Corridor.cpp
    Door.cpp
    Model.cpp
    Room.cpp
    SVGUtils.cpp
)

target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET "${PROJECT_NAME}_HEADERS"
    TYPE HEADERS
    BASE_DIRS
        "../"
    FILES
        "../model/Corridor.h"
        "../model/Door.h"
        "../model/Model.h"
        "../model/Room.h"
        "../model/Variables.h"
)

# Use libraries installed by conan
if(EXISTS "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
endif()

find_package(svgwrite REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        analytical-solver
        svgwrite::svgwrite
)
