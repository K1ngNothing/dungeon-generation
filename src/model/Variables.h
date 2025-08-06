#pragma once

#include <cstddef>
#include <optional>
#include <vector>

namespace DungeonGeneration {
namespace Model {

struct VariablesIds {
    size_t xId;
    size_t yId;
};

struct Variables {
    double varX;
    double varY;
};

struct Interval {
    double lowerBound;
    double upperBound;
};
using VariablesBounds = std::vector<std::optional<Interval>>;

namespace VarUtils {
VariablesIds getVariablesIds(size_t id);
Variables getVariablesVal(const double* x, size_t id);

}  // namespace VarUtils

class ObjectWithVars {
public:
    ObjectWithVars() = default;
    explicit ObjectWithVars(size_t id);

    VariablesIds getVariablesIds() const;
    Variables getVariablesVal(const double* x) const;

protected:
    size_t varObjId_;
};

struct ObjectMaybeWithVars {
public:
    explicit ObjectMaybeWithVars(std::optional<size_t> id = std::nullopt);

    bool hasVariables() const;
    VariablesIds getVariablesIds() const;
    Variables getVariablesVal(const double* x) const;

protected:
    std::optional<size_t> varObjId_;
};

}  // namespace Model
}  // namespace DungeonGeneration
