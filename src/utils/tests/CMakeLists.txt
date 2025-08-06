cmake_minimum_required(VERSION 3.23)

project(utils_test)

add_executable(${PROJECT_NAME}
    RandomTests.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    GTest::gtest_main
    utils
)

enable_testing()

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
