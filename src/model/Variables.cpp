#include "Variables.h"

#include <cassert>

namespace DungeonGeneration {
namespace Model {

namespace VarUtils {

VariablesIds getVariablesIds(size_t id)
{
    return VariablesIds{.xId = id * 2, .yId = id * 2 + 1};
}

Variables getVariablesVal(const double* x, size_t id)
{
    assert(x && "Null variables array");
    const auto [xId, yId] = getVariablesIds(id);
    return Variables{.varX = x[xId], .varY = x[yId]};
}

}  // namespace VarUtils

// ---------------------------------------------------------------------------------------------------------------------
// ----- ObjectWithVars -----

ObjectWithVars::ObjectWithVars(size_t id)
      : varObjId_(id)
{}

VariablesIds ObjectWithVars::getVariablesIds() const
{
    return VarUtils::getVariablesIds(varObjId_);
}

Variables ObjectWithVars::getVariablesVal(const double* x) const
{
    return VarUtils::getVariablesVal(x, varObjId_);
}

// ---------------------------------------------------------------------------------------------------------------------
// ----- ObjectMaybeWithVars -----

ObjectMaybeWithVars::ObjectMaybeWithVars(std::optional<size_t> id)
      : varObjId_(id)
{}

bool ObjectMaybeWithVars::hasVariables() const
{
    return varObjId_.has_value();
}

VariablesIds ObjectMaybeWithVars::getVariablesIds() const
{
    assert(hasVariables() && "getVariablesIds: object must contain variables");
    return VarUtils::getVariablesIds(varObjId_.value());
}

Variables ObjectMaybeWithVars::getVariablesVal(const double* x) const
{
    assert(hasVariables() && "getVariablesVal: object must contain variables");
    return VarUtils::getVariablesVal(x, varObjId_.value());
}

}  // namespace Model
}  // namespace DungeonGeneration
