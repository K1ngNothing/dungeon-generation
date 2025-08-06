#include "SVGDumper.h"

#include <cassert>

namespace DungeonGeneration {
namespace Callbacks {

SVGDumper::SVGDumper(Model::Model& model, const std::filesystem::path& pathToSVG, const std::string& filenamePrefix)
      : model_(model),
        pathToSVG_(pathToSVG),
        filenamePrefix_(filenamePrefix)
{}

void SVGDumper::operator()(const double* x, int runNum, int iterNum)
{
    assert(x && "SVGDumper::operator(): Null variables array");

    // Note that we DO change model_ in here by resetting stored rooms' positions.
    // Idea is, that these positions don't mean anything while solver runs, and are resetted at the solving end.

    // TODO: reuse code from AnalyticalSolver::retrieveSolution
    const size_t objectCount = model_.getObjectCount();
    Model::Positions positions(objectCount);
    for (size_t objId = 0; objId < objectCount; ++objId) {
        const auto [xId, yId] = Model::VarUtils::getVariablesIds(objId);
        positions[objId].x = x[xId];
        positions[objId].y = x[yId];
    }
    model_.setPositions(positions);
    const std::string filename = filenamePrefix_ + "_" + std::to_string(runNum) + "_" + std::to_string(iterNum);
    model_.dumpToSVG(pathToSVG_ / (filename + ".svg"));
}

}  // namespace Callbacks
}  // namespace DungeonGeneration
