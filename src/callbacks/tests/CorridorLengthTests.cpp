#include <gtest/gtest.h>

#include <random>

#include <callbacks/CorridorLength.h>
#include <model/Room.h>

#include "Common.h"

using namespace DungeonGenerator;

TEST(CallbacksTests, CorridorLengthGradientTest)
{
    Model::Room room1{0, 10, 10, {}, std::nullopt};
    Model::Room room2{1, 20, 20, {}, std::nullopt};
    Model::Door door1{10, 0, 0};
    Model::Door door2{0, 20, 1};
    Callbacks::CorridorLength corridorLength(door1, door2);

    std::uniform_real_distribution<double> varDistribution(-50, 50);
    std::mt19937 rng(42);

    constexpr size_t iterCount = 10000;
    for (size_t it = 0; it < iterCount; ++it) {
        std::vector<double> x(4, 0.0);    // First room is always in (0, 0)
        for (size_t i = 2; i < 4; ++i) {  // Second room has random position
            x[i] = varDistribution(rng);
        }
        checkGradientCorrectness(corridorLength, x);
    }
}
