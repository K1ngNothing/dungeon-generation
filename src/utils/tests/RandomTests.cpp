#include <gtest/gtest.h>

#include <utils/Random.h>

using namespace DungeonGeneration;

TEST(RandomTests, TestGenerateUniformDiscrete)
{
    constexpr size_t n = 5;
    constexpr size_t iterCount = 1e5;
    std::vector<size_t> counts(n);
    Random::RNG rng(42);
    for (size_t it = 0; it < iterCount; ++it) {
        size_t id = Random::uniformDiscrete(n - 1, rng);
        counts[id]++;
    }

    constexpr double tolerance = 0.05;
    for (size_t i = 0; i < n - 1; ++i) {
        const double ratio = static_cast<double>(counts[i]) / counts[i + 1];
        EXPECT_NEAR(ratio, 1.0, tolerance);
    }
}

TEST(RandomTests, TestGenerateUniformContinuous)
{
    constexpr size_t n = 5;
    constexpr size_t iterCount = 1e5;
    std::vector<size_t> counts(n);
    Random::RNG rng(42);
    for (size_t it = 0; it < iterCount; ++it) {
        const double num = Random::uniformRangeContinuous<double>(0, n, rng);
        assert(num >= 0.0);
        const size_t bucket = static_cast<size_t>(num);
        counts[bucket]++;
    }

    constexpr double tolerance = 0.05;
    for (size_t i = 0; i < n - 1; ++i) {
        const double ratio = static_cast<double>(counts[i]) / counts[i + 1];
        EXPECT_NEAR(ratio, 1.0, tolerance);
    }
}

TEST(RandomTests, TestGenerateFromDistribution)
{
    std::vector<double> weights{1.0, 1.5, 0.5};

    constexpr size_t iterCount = 1e5;
    std::vector<size_t> counts(weights.size());
    Random::RNG rng(42);
    for (size_t it = 0; it < iterCount; ++it) {
        size_t id = Random::fromDistribution(weights, rng);
        counts[id]++;
    }

    constexpr double tolerance = 0.05;
    const double ratio1 = static_cast<double>(counts[0]) / counts[2];
    const double ratio2 = static_cast<double>(counts[1]) / counts[2];
    EXPECT_NEAR(ratio1, 2.0, tolerance);
    EXPECT_NEAR(ratio2, 3.0, tolerance);
}
