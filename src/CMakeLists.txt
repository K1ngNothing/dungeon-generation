cmake_minimum_required(VERSION 3.23)

project(dungeon-generator)
add_executable(${PROJECT_NAME}
    main.cpp
)

# Set PETSc variables according to build type
set(PETSC_DIR "../petsc")
set(PETSC_LIBS petsc mpi mpicxx f2clapack f2cblas)
string(TOLOWER ${CMAKE_BUILD_TYPE} _build_type)
if(_build_type STREQUAL "release")
    set(PETSC_ARCH "arch-linux-c-opt")
else()
    set(PETSC_ARCH "arch-linux-c-debug")
endif()
unset(_build_type)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${PETSC_DIR}/include"
        "${PETSC_DIR}/${PETSC_ARCH}/include"
)

target_link_directories(${PROJECT_NAME}
    PRIVATE
        "${PETSC_DIR}/${PETSC_ARCH}/lib"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${PETSC_LIBS}
)
