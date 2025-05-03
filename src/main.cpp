#include <DungeonGenerator.h>
#include <utils/CLArguments.h>

using namespace DungeonGeneration;

int main(int argc, char** argv)
{
    CLUtils::provideCLArgumentsHandler().setArguments(argc, argv);
    DungeonGenerator dungeonGenerator;
    static_cast<void>(dungeonGenerator.generateDungeon());  // Dumps SVG inside DungeonGenerator
    return 0;
}
