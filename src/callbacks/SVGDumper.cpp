#include "SVGDumper.h"

#include <cassert>

namespace DungeonGenerator {
namespace Callbacks {

SVGDumper::SVGDumper(Model::Model& model, const std::filesystem::path& pathToSVG, const std::string& filenamePrefix)
      : model_(model),
        pathToSVG_(pathToSVG),
        filenamePrefix_(filenamePrefix)
{}

void SVGDumper::operator()(const double* x, int iterNum)
{
    // Note that we DO change model_ in here by resetting stored rooms' positions.
    // Idea is, that these positions don't mean anything while solver runs, and are resetted at the solving end.

    const Model::Rooms& rooms = model_.getRooms();
    Model::Positions positions(rooms.size());
    for (size_t i = 0; i < rooms.size(); ++i) {
        const auto [valX, valY] = rooms[i].getVariables(x);
        positions[i] = Model::Position{.x = valX, .y = valY};
    }
    model_.setPositions(positions);
    model_.dumpToSVG(pathToSVG_ / (filenamePrefix_ + "_" + std::to_string(iterNum) + ".svg"));
}

}  // namespace Callbacks
}  // namespace DungeonGenerator
