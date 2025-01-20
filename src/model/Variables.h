#pragma once

#include <cstddef>

namespace DungeonGenerator {
namespace Model {

struct Variables {
    double varX;
    double varY;
};

struct VariablesRef {
    double& varX;
    double& varY;
};

struct VariablesIds {
    size_t xId;
    size_t yId;
};

namespace Utils {

// TODO: maybe create a base class `ObjectWithVars` and inherit Door and Room from it?

inline Variables getVariables(size_t roomId, const double* x)
{
    return Variables{.varX = x[roomId * 2], .varY = x[roomId * 2 + 1]};
}

inline VariablesRef getVariablesRef(size_t roomId, double* x)
{
    return VariablesRef{.varX = x[roomId * 2], .varY = x[roomId * 2 + 1]};
}

}  // namespace Utils

}  // namespace Model
}  // namespace DungeonGenerator
