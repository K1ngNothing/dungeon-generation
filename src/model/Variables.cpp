#include "Variables.h"

namespace DungeonGeneration {
namespace Model {

namespace VarUtils {

VariablesIds getVariablesIds(size_t id)
{
    return VariablesIds{.xId = id * 2, .yId = id * 2 + 1};
}

Variables getVariablesVal(const double* x, size_t id)
{
    const auto [xId, yId] = getVariablesIds(id);
    return Variables{.varX = x[xId], .varY = x[yId]};
}

}  // namespace VarUtils

VariablesIds ObjectWithVars::getVariablesIds() const
{
    return VarUtils::getVariablesIds(id);
}

Variables ObjectWithVars::getVariablesVal(const double* x) const
{
    return VarUtils::getVariablesVal(x, id);
}

}  // namespace Model
}  // namespace DungeonGeneration
