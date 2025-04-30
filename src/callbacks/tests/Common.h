#include <gtest/gtest.h>

#include <callbacks/Defs.h>

using namespace DungeonGeneration;

inline void checkGradientCorrectness(Callbacks::FGEval func, std::vector<double> x)
{
    size_t n = x.size();
    double f0 = 0.0;
    std::vector<double> gradient0(n, 0.0);
    func(x.data(), f0, gradient0.data());
    constexpr double dx = 1e-4, tolerance = 5e-3;
    for (size_t i = 0; i < n; ++i) {
        x[i] += dx;
        double f = 0.0;
        func(x.data(), f, nullptr);
        double empiricDx = (f - f0) / dx;
        EXPECT_NEAR(empiricDx, gradient0[i], tolerance) << "Incorrect gradient";
        x[i] -= dx;
    }
}
