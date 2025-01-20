#include <iostream>

#include <AnalyticalSolver.h>
#include <functions/CorridorLength.h>
#include <functions/RoomOverlap.h>
#include <ModelGenerator.h>

// TODO: wrap flow in a proper class (e.g. DungeonGenerator)
// TODO: handle exceptions
int main()
{
    using namespace DungeonGenerator;

    // 1. Generate model
    ModelGenerator modelGenerator;
    constexpr size_t gridSide = 5;
    Model::Model model = modelGenerator.generateGrid(gridSide);

    // 2. Create functions
    std::vector<Functions::FGEval> costFunctions;
    for (const auto& [door1, door2] : model.corridors) {
        costFunctions.push_back(Functions::CorridorLength(door1, door2));
    }
    std::vector<Functions::FGEval> penaltyFunctions;
    for (size_t i = 0; i < model.rooms.size(); ++i) {
        for (size_t j = i + 1; j < model.rooms.size(); ++j) {
            penaltyFunctions.push_back(Functions::RoomOverlap(model.rooms[i], model.rooms[j]));
        }
    }

    // 3. Create and run a solver
    AnalyticalSolver::AnalyticalSolver solver(
        model.rooms.size(), std::move(costFunctions), std::move(penaltyFunctions));
    solver.solve();
    AnalyticalSolver::Solution solution = solver.retrieveSolution();
    for (size_t i = 0; i < gridSide; ++i) {
        for (size_t j = 0; j < gridSide; ++j) {
            AnalyticalSolver::Position position = solution[i * gridSide + j];
            std::cout << position.x << ' ' << position.y << ' ';
        }
        std::cout << "\n";
    }
    return 0;
}
