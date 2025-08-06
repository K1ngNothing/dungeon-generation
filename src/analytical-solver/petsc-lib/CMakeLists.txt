# An empty lib that publicly links PETSc TAO library. Used to easily link the aforementioned library to other targets.

cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME petsc_lib) # For some reason this variable won't be automatically be set
add_library(${PROJECT_NAME} INTERFACE)

# Check whether user specified PETSc variables. Use defaults if not set.
set(PETSC_DIR "${CMAKE_BINARY_DIR}/../../petsc" CACHE PATH "Path to PETSc directory")
set(PETSC_ARCH "" CACHE STRING "PETSc build name (default oт linux: arch-linux-c-debug for Debug, arch-linux-c-opt for Release).")

# If PETSC_ARCH wasn't set by user, set it based on the build type.
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

# Verify that PETSc exists at the given path
if(NOT EXISTS "${PETSC_DIR}/${PETSC_ARCH}")
    message(FATAL_ERROR "PETSc not found at ${PETSC_DIR}/${PETSC_ARCH}")
endif()

# Link PETSc libraries based on variables (plus project's libraries)
set(PETSC_LIBS petsc mpi mpicxx f2clapack f2cblas)
target_include_directories(${PROJECT_NAME}
    INTERFACE
        "${PETSC_DIR}/include"
        "${PETSC_DIR}/${PETSC_ARCH}/include"
)

target_link_directories(${PROJECT_NAME}
    INTERFACE
        "${PETSC_DIR}/${PETSC_ARCH}/lib"
)

target_link_libraries(${PROJECT_NAME}
    INTERFACE
        ${PETSC_LIBS}
)
