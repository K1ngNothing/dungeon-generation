cmake_minimum_required(VERSION 3.23)

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

# Check whether user specified PETSc variables. Use defaults if not set.
set(PETSC_DIR "../../petsc" CACHE PATH "Path to PETSc directory")
set(PETSC_ARCH "" CACHE STRING "PETSc build name (default oт linux: arch-linux-c-debug for Debug, arch-linux-c-opt for Release).")

# If PETSC_ARCH wasn't set by user, set it based on build type.
if(NOT PETSC_ARCH)
    string(TOLOWER ${CMAKE_BUILD_TYPE} _build_type)

    if(_build_type STREQUAL "release")
        set(PETSC_ARCH "arch-linux-c-opt")
    else()
        set(PETSC_ARCH "arch-linux-c-debug")
    endif()

    unset(_build_type)
    message(STATUS "PETSC_ARCH not set, defaulting to ${PETSC_ARCH} (based on CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})")
endif()

# Link PETSc libraries based on variables (plus project's libraries)
set(PETSC_LIBS petsc mpi mpicxx f2clapack f2cblas)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${PETSC_DIR}/include"
        "${PETSC_DIR}/${PETSC_ARCH}/include"
)

target_link_directories(${PROJECT_NAME}
    PUBLIC
        "${PETSC_DIR}/${PETSC_ARCH}/lib"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${PETSC_LIBS}
        callbacks
        utils
)
