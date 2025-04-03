#pragma once

#include <random>

#include <model/Model.h>

namespace DungeonGenerator {
namespace Callbacks {

class RoomShaker {
public:
    RoomShaker(const Model::Model& model);
    void operator()(double* x);

private:
    double generateUniform(double lb, double rb);

    const Model::Model& model_;

    static constexpr size_t kSeed = 42;
    std::mt19937 rng_ = std::mt19937(kSeed);  // random number generator
};

}  // namespace Callbacks
}  // namespace DungeonGenerator
