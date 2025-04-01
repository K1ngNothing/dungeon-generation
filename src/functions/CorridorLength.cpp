#include "CorridorLength.h"

namespace DungeonGenerator {
namespace Functions {

CorridorLength::CorridorLength(const Model::Door& door1, const Model::Door& door2)
      : door1_(door1),
        door2_(door2)
{}

void CorridorLength::operator()(const double* x, double& f, double* grad) const
{
    const auto [x1, y1] = door1_.getVariables(x);
    const auto [x2, y2] = door2_.getVariables(x);
    const auto [x1Id, y1Id] = door1_.getVariablesIds();
    const auto [x2Id, y2Id] = door2_.getVariablesIds();

    const double dx = x1 - x2;
    const double dy = y1 - y2;
    f += dx * dx + dy * dy;

    if (grad != nullptr) {
        const double gradX1 = 2 * dx;
        const double gradY1 = 2 * dy;
        grad[x1Id] += gradX1;
        grad[y1Id] += gradY1;
        grad[x2Id] -= gradX1;
        grad[y2Id] -= gradY1;
    }
}

}  // namespace Functions
}  // namespace DungeonGenerator
