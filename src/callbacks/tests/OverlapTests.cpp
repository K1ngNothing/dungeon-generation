#include <gtest/gtest.h>

#include <random>

#include <callbacks/RoomOverlap.h>

#include "Common.h"

using namespace DungeonGenerator;

TEST(CallbacksTests, OverlapValueTest)
{
    constexpr double tolerance = 1e-9;
    Model::Room room1{0, 10, 20, {}, std::nullopt};
    Model::Room room2{1, 20, 10, {}, std::nullopt};
    Callbacks::RoomOverlap overlap(room1, room2);
    std::vector<double> xVec;
    double val = 0.0;

    // Cases where rooms do not intersect
    xVec = {0, 0, 15, 0};
    overlap(xVec.data(), val, nullptr);
    EXPECT_NEAR(val, 0.0, tolerance) << "Incorrect overlap value: " << val;

    xVec = {0, 0, 0, 15};
    overlap(xVec.data(), val, nullptr);
    EXPECT_NEAR(val, 0.0, tolerance) << "Incorrect overlap value: " << val;

    xVec = {0, 0, 100, 100};
    overlap(xVec.data(), val, nullptr);
    EXPECT_NEAR(val, 0.0, tolerance) << "Incorrect overlap value: " << val;

    // Rooms are stacked right on top of each other
    xVec = {0, 0, 0, 0};
    overlap(xVec.data(), val, nullptr);
    EXPECT_NEAR(val, 1.0, tolerance) << "Incorrect overlap value: " << val;

    // Some random point, value is calculated by hand
    val = 0.0;
    xVec = {0, 0, 10, 10};
    overlap(xVec.data(), val, nullptr);
    EXPECT_NEAR(val, 0.095259868, tolerance) << "Incorrect overlap value: " << val;
}

TEST(CallbacksTests, OverlapGradientTest)
{
    Model::Room room1{0, 10, 10, {}, std::nullopt};
    Model::Room room2{1, 20, 20, {}, std::nullopt};
    Callbacks::RoomOverlap overlap(room1, room2);

    std::uniform_real_distribution<double> varDistribution(-17, 17);  // To have a chance of no intersection
    std::mt19937 rng(42);

    constexpr size_t iterCount = 100;
    for (size_t it = 0; it < iterCount; ++it) {
        std::vector<double> x(4, 0.0);    // First room is always in (0, 0)
        for (size_t i = 2; i < 4; ++i) {  // Second room has random position
            x[i] = varDistribution(rng);
        }
        checkGradientCorrectness(overlap, x);
    }
}
