cmake_minimum_required(VERSION 3.23)

add_subdirectory(analytical-solver)
add_subdirectory(callbacks)
add_subdirectory(model)
add_subdirectory(model-generator)

project(dungeon-generator)
add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        analytical-solver
        callbacks
        model
        model-generator
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -DPATH_TO_SVG="${CMAKE_BINARY_DIR}"
)
