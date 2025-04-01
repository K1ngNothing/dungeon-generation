#include <filesystem>

#include <AnalyticalSolver.h>
#include <functions/CorridorLength.h>
#include <functions/RoomOverlap.h>
#include <model/Model.h>
#include <ModelGenerator.h>

std::filesystem::path kPathToSVG
{
#if defined(PATH_TO_SVG)
    PATH_TO_SVG
#endif
};

// TODO: wrap flow in a proper class (e.g. DungeonGenerator)
// TODO: handle exceptions
int main()
{
    using namespace DungeonGenerator;

    // 1. Generate model
    ModelGenerator modelGenerator;
    constexpr size_t gridSide = 5;
    Model::Model gridModel = modelGenerator.generateGrid(gridSide);
    gridModel.dumpToSVG({}, kPathToSVG / "grid_input.svg");

    // 2. Create functions
    std::vector<Functions::FGEval> costFunctions;
    for (const auto& [door1, door2] : gridModel.getCorridors()) {
        costFunctions.push_back(Functions::CorridorLength(door1, door2));
    }
    std::vector<Functions::FGEval> penaltyFunctions;
    const Model::Rooms& rooms = gridModel.getRooms();
    for (size_t i = 0; i < rooms.size(); ++i) {
        for (size_t j = i + 1; j < rooms.size(); ++j) {
            penaltyFunctions.push_back(Functions::RoomOverlap(rooms[i], rooms[j]));
        }
    }

    // 3. Create and run a solver
    AnalyticalSolver::AnalyticalSolver solver(rooms.size(), std::move(costFunctions), std::move(penaltyFunctions));
    solver.solve();
    Model::Positions solution = solver.retrieveSolution();
    gridModel.setPositions(solution);

    // 4. Dump results
    gridModel.dumpToSVG({}, kPathToSVG / "grid_result.svg");

    // DEBUG
    // 5. Reevaluate functions
    // std::vector<double> xVec;
    // for (const auto [x, y] : solution) {
    //     xVec.push_back(x);
    //     xVec.push_back(y);
    // }
    // std::cout << "TWL: ";
    // for (const auto& [door1, door2] : gridModel.getCorridors()) {
    //     auto func = Functions::CorridorLength(door1, door2);
    //     double val = 0.0;
    //     func(xVec.data(), val, nullptr);
    //     std::cout << val << ' ';
    // }
    // std::cout << "\nOverlap:\n";
    // for (size_t i = 0; i < rooms.size(); ++i) {
    //     for (size_t j = 0; j < rooms.size(); ++j) {
    //         if (i == j) {
    //             std::cout << 1.0 << ' ';
    //             continue;
    //         }
    //         auto func = Functions::RoomOverlap(rooms[i], rooms[j]);
    //         double val = 0.0;
    //         func(xVec.data(), val, nullptr);
    //         std::cout << val << ' ';
    //     }
    //     std::cout << "\n";
    // }

    return 0;
}
