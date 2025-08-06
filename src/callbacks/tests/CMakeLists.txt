cmake_minimum_required(VERSION 3.23)

project(callbacks_test)

add_executable(${PROJECT_NAME}
    CorridorLengthTests.cpp
    OverlapTests.cpp
    PushForceTests.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    GTest::gtest_main
    callbacks
    utils
)

enable_testing()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
