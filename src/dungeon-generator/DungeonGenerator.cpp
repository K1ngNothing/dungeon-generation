#include "DungeonGenerator.h"

#include <cassert>

#include <AnalyticalSolver.h>
#include <callbacks/CorridorLength.h>
#include <callbacks/PushForce.h>
#include <callbacks/RoomOverlap.h>
#include <callbacks/RoomShaker.h>
#include <callbacks/SVGDumper.h>

std::filesystem::path kPathToSVG
{
#if defined(PATH_TO_SVG)
    PATH_TO_SVG
#endif
};

namespace DungeonGeneration {

Model::Model DungeonGenerator::generateDungeon() const
{
    Model::Model model = generateModel();
    model = runSolver(std::move(model));
    model.dumpToSVG(kPathToSVG / "result.svg");
    return model;
}

Model::Model DungeonGenerator::generateModel() const
{
    ModelGenerator modelGenerator;
    switch (kDungeonType) {
        case DungeonType::Grid: {
            Model::Model model = modelGenerator.generateGrid(kGridSide);
            model.dumpToSVG(kPathToSVG / "grid_input.svg");
            return model;
        }
        case DungeonType::TreeFixedDoors:
            return modelGenerator.generateTreeFixedDoors(kRoomCount);
        case DungeonType::TreeMovableDoors:
            return modelGenerator.generateTreeMovableDoors(kRoomCount);
        default:
            assert(false && "Unsupported DungeonType");
            return Model::Model();
    }
}

Model::Model DungeonGenerator::runSolver(Model::Model&& model) const
{
    // Cost functions
    std::vector<Callbacks::FGEval> costFunctions;
    for (const auto& [door1, door2] : model.getCorridors()) {
        costFunctions.push_back(Callbacks::CorridorLength(door1, door2));
    }
    if (kEnablePushForce) {
        constexpr double scale = 10.0;
        constexpr double range = 5.0;
        costFunctions.push_back(Callbacks::PushForce(model, scale, range));
    }

    // Penalty functions
    std::vector<Callbacks::FGEval> penaltyFunctions;
    const Model::Rooms& rooms = model.getRooms();
    for (size_t i = 0; i < rooms.size(); ++i) {
        for (size_t j = i + 1; j < rooms.size(); ++j) {
            penaltyFunctions.push_back(Callbacks::RoomOverlap(rooms[i], rooms[j]));
        }
    }

    // On iteration callbacks
    std::vector<Callbacks::ModifierCallback> modifierCallbacks{Callbacks::RoomShaker(model)};
    std::vector<Callbacks::ReaderCallback> readerCallbacks{Callbacks::SVGDumper(model, kPathToSVG, "iter")};

    // Create and run a analytical solver
    AnalyticalSolver::AnalyticalSolver solver(
        model.getObjectCount(), model.getVariablesCount(), model.getVariablesBounds(), std::move(costFunctions),
        std::move(penaltyFunctions), std::move(modifierCallbacks), std::move(readerCallbacks));
    solver.solve();
    Model::Positions solution = solver.retrieveSolution();
    model.setPositions(solution);

    return std::move(model);
}

}  // namespace DungeonGeneration
