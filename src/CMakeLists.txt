cmake_minimum_required(VERSION 3.23)

add_subdirectory(analytical-solver)
add_subdirectory(callbacks)
add_subdirectory(dungeon-generator)
add_subdirectory(model)
add_subdirectory(model-generator)
add_subdirectory(utils)

project(run_dungeon_generator)
add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        dungeon_generator
        utils
)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest"
    GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(googletest)
