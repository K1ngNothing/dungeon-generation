#include <gtest/gtest.h>

#include <random>

#include <callbacks/PushForce.h>

#include "Common.h"

using namespace DungeonGenerator;

TEST(CallbacksTests, PushForceValueTest)
{
    constexpr double tolerance = 1e-9;
    Model::Room room1{0, 10, 10, {}, std::nullopt};
    Model::Room room2{1, 20, 20, {}, std::nullopt};
    Model::Rooms rooms = {room1, room2};
    Model::Model model(std::move(rooms), {});
    Callbacks::PushForce pushForce(model);

    // Rooms are right on top of each other
    std::vector<double> x(4, 0.0);
    double val = 0.0;
    pushForce(x.data(), val, nullptr);
    EXPECT_NEAR(val, 1.0, tolerance) << "Incorrect overlap value: " << val;

    // Some random point, value is calculated by hand
    x = {0.0, 0.0, 15, 20};
    val = 0.0;
    pushForce(x.data(), val, nullptr);
    EXPECT_NEAR(val, 0.2647058823529412, tolerance) << "Incorrect overlap value: " << val;

    // Test that scale works
    Callbacks::PushForce pushForce2(model, 2.5);
    x = {0.0, 0.0, 15, 20};
    val = 0.0;
    pushForce2(x.data(), val, nullptr);
    EXPECT_NEAR(val, 0.6617647058823529, tolerance) << "Incorrect overlap value: " << val;
}

TEST(CallbacksTests, PushForceGradientTest)
{
    Model::Room room1{0, 10, 10, {}, std::nullopt};
    Model::Room room2{1, 20, 20, {}, std::nullopt};
    Model::Rooms rooms = {room1, room2};
    Model::Model model(std::move(rooms), {});
    Callbacks::PushForce pushForce(model);

    std::uniform_real_distribution<double> varDistribution(-50, 50);
    std::mt19937 rng(42);

    constexpr size_t iterCount = 1000;
    for (size_t it = 0; it < iterCount; ++it) {
        std::vector<double> x(4, 0.0);    // First room is always in (0, 0)
        for (size_t i = 2; i < 4; ++i) {  // Second room has random position
            x[i] = varDistribution(rng);
        }
        checkGradientCorrectness(pushForce, x);
    }
}
