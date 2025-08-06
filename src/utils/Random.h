#pragma once

#include <cassert>
#include <random>

namespace DungeonGeneration {
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

inline size_t fromDistribution(const std::vector<double>& weights, RNG& rng)
{
    std::discrete_distribution<size_t> generator(weights.begin(), weights.end());
    size_t result = generator(rng);
    assert(result < weights.size() && "Invalid generation");
    return result;
}

}  // namespace Random
}  // namespace DungeonGeneration
