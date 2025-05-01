#include <gtest/gtest.h>

#include <callbacks/CorridorLength.h>
#include <model/Room.h>
#include <random/Random.h>

#include "Common.h"

using namespace DungeonGeneration;

TEST(CallbacksTests, CorridorLengthGradientTest)
{
    Model::Room room1(0, 10, 10, {});
    Model::Room room2(1, 20, 20, {});
    Model::Door door1 = Model::Door::createFixedDoor(0, {10, 0});
    Model::Door door2 = Model::Door::createFixedDoor(0, {20, 1});
    Callbacks::CorridorLength corridorLength(door1, door2);

    constexpr size_t iterCount = 10000;
    Random::RNG rng(42);
    for (size_t it = 0; it < iterCount; ++it) {
        std::vector<double> x(4, 0.0);    // First room is always in (0, 0)
        for (size_t i = 2; i < 4; ++i) {  // Second room has a random position
            x[i] = Random::uniformRangeContinuous(-50.0, 50.0, rng);
        }
        checkGradientCorrectness(corridorLength, x);
    }
}
