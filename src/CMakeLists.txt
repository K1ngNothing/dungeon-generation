cmake_minimum_required(VERSION 3.23)

add_subdirectory(analytical-solver)
add_subdirectory(functions)
add_subdirectory(model)
add_subdirectory(model-generator)

project(dungeon-generator)
add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        analytical-solver
        functions
        model-generator
)
