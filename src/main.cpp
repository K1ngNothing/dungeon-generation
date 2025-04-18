#include <filesystem>
#include <iostream>

#include <AnalyticalSolver.h>
#include <callbacks/CorridorLength.h>
#include <callbacks/PushForce.h>
#include <callbacks/RoomOverlap.h>
#include <callbacks/RoomShaker.h>
#include <callbacks/SVGDumper.h>
#include <model/Model.h>
#include <ModelGenerator.h>

std::filesystem::path kPathToSVG
{
#if defined(PATH_TO_SVG)
    PATH_TO_SVG
#endif
};
using namespace DungeonGenerator;

// Settings
constexpr bool kEnablePushForce = true;

void runSolver(Model::Model& model)
{
    // 1. Create callbacks
    std::vector<Callbacks::FGEval> costFunctions;
    for (const auto& [door1, door2] : model.getCorridors()) {
        costFunctions.push_back(Callbacks::CorridorLength(door1, door2));
    }
    if (kEnablePushForce) {
        costFunctions.push_back(Callbacks::PushForce(model, 10, 5));
    }

    std::vector<Callbacks::FGEval> penaltyFunctions;
    const Model::Rooms& rooms = model.getRooms();
    for (size_t i = 0; i < rooms.size(); ++i) {
        for (size_t j = i + 1; j < rooms.size(); ++j) {
            penaltyFunctions.push_back(Callbacks::RoomOverlap(rooms[i], rooms[j]));
        }
    }

    std::vector<Callbacks::ModifierCallback> modifierCallbacks{Callbacks::RoomShaker(model)};
    std::vector<Callbacks::ReaderCallback> readerCallbacks{Callbacks::SVGDumper(model, kPathToSVG, "iter")};

    // 2. Create and run a analytical solver
    AnalyticalSolver::AnalyticalSolver solver(
        model.getObjectCount(), model.getVariablesCount(), model.getVariablesBounds(), std::move(costFunctions),
        std::move(penaltyFunctions), std::move(modifierCallbacks), std::move(readerCallbacks));
    solver.solve();
    Model::Positions solution = solver.retrieveSolution();
    model.setPositions(solution);
}

void runGrid()
{
    // 1. Generate model
    ModelGenerator modelGenerator;
    constexpr size_t gridSide = 5;
    Model::Model model = modelGenerator.generateGrid(gridSide);
    model.dumpToSVG(kPathToSVG / "grid_input.svg");

    // 2. Run solver
    runSolver(model);

    // 3. Dump results
    model.dumpToSVG(kPathToSVG / "grid_result.svg");
}

void runTree()
{
    // 1. Generate model
    ModelGenerator modelGenerator;
    constexpr size_t roomCount = 50;
    Model::Model model = modelGenerator.generateTreeCenterRooms(roomCount);
    std::cout << "Tree edges:\n";
    for (const Model::Corridor& corridor : model.getCorridors()) {
        size_t v = corridor.door1.parentRoomId;
        size_t u = corridor.door2.parentRoomId;
        std::cout << v << ' ' << u << "\n";
    }

    // 2. Run solver
    runSolver(model);

    // 3. Dump results
    model.dumpToSVG(kPathToSVG / "tree_result.svg");
}

// TODO: wrap flow in a proper class (e.g. DungeonGenerator)
// TODO: handle exceptions
int main()
{
    // runGrid();
    runTree();
    return 0;
}
