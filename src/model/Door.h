#pragma once

#include "Variables.h"

namespace DungeonGenerator {
namespace Model {

struct Door {
    double dx;
    double dy;
    size_t roomId;

    Variables getVariables(const double* x) const;
    VariablesIds getVariablesIds() const;
};

}  // namespace Model
}  // namespace DungeonGenerator
