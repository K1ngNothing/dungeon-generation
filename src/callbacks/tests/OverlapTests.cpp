#include <gtest/gtest.h>

#include <callbacks/RoomOverlap.h>
#include <petsc.h>
#include <utils/Random.h>

#include "Common.h"

using namespace DungeonGeneration;

// A wrapper class that uses non PETSc TAO interface
class OverlapWrapper {
public:
    OverlapWrapper(const Model::Room& room1, const Model::Room& room2)
          : overlap_(room1, room2, 1.0)
    {
        PetscInitializeNoArguments();
        MatCreateSeqDense(PETSC_COMM_SELF, cEqCnt_, varCnt_, nullptr, &JEq_);
    }

    void operator()(const double* x, double& f, double* grad) const
    {
        MatZeroEntries(JEq_);
        overlap_(x, f, JEq_, 0);
        MatAssemblyBegin(JEq_, MAT_FINAL_ASSEMBLY);
        MatAssemblyEnd(JEq_, MAT_FINAL_ASSEMBLY);

        if (!grad) {
            return;
        }
        int colCount;
        const double* JEqArr;
        MatGetRow(JEq_, 0, &colCount, nullptr, &JEqArr);
        assert(colCount == varCnt_);
        for (size_t i = 0; i < varCnt_; ++i) {
            grad[i] += JEqArr[i];
        }
    }

private:
    Callbacks::RoomOverlap overlap_;
    static constexpr size_t cEqCnt_ = 1;
    static constexpr size_t varCnt_ = 4;
    Mat JEq_;
};

TEST(CallbacksTests, OverlapValueTest)
{
    constexpr double tolerance = 1e-9;
    Model::Room room1(0, 10, 20, {});
    Model::Room room2(1, 20, 10, {});
    OverlapWrapper overlap(room1, room2);
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
    Model::Room room1(0, 10, 10, {});
    Model::Room room2(1, 20, 20, {});
    OverlapWrapper overlap(room1, room2);

    constexpr size_t iterCount = 3000;
    Random::RNG rng(42);
    for (size_t it = 0; it < iterCount; ++it) {
        std::vector<double> x(4, 0.0);    // First room is always in (0, 0)
        for (size_t i = 2; i < 4; ++i) {  // Second room has a random position
            x[i] = Random::uniformRangeContinuous(-17.0, 17.0, rng);
        }
        checkGradientCorrectness(overlap, x);
    }
}
