#pragma once

#include <cstddef>

namespace DungeonGenerator {
namespace Model {

struct VariablesIds {
    size_t xId;
    size_t yId;
};

struct Variables {
    double varX;
    double varY;
};

namespace VarUtils {

VariablesIds getVariablesIds(size_t id);
Variables getVariablesVal(const double* x, size_t id);

}  // namespace VarUtils

struct ObjectWithVars {
    size_t id;

    VariablesIds getVariablesIds() const;
    Variables getVariablesVal(const double* x) const;
};

}  // namespace Model
}  // namespace DungeonGenerator
