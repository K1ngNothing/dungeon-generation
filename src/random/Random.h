#pragma once

#include <cassert>
#include <random>

namespace DungeonGenerator {
namespace Random {

using RNG = std::mt19937;
constexpr size_t kGlobalSeed = 42;

template <typename T>
T uniformRangeDiscrete(T lb, T rb, RNG& rng)
{
    assert(lb <= rb && "Invalid range");
    std::uniform_int_distribution<T> distribution(lb, rb);
    return distribution(rng);
}

template <typename T>
T uniformDiscrete(T upperLimit, RNG& rng)
{
    return uniformRangeDiscrete<T>(0, upperLimit, rng);
}

template <typename T>
T uniformRangeContinuous(T lb, T rb, RNG& rng)
{
    assert(lb <= rb && "Invalid range");
    std::uniform_real_distribution<T> distribution(lb, rb);
    return distribution(rng);
}

}  // namespace Random
}  // namespace DungeonGenerator
