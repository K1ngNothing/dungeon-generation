#include <DungeonGenerator.h>

using namespace DungeonGeneration;

int main()
{
    DungeonGenerator dungeonGenerator;
    static_cast<void>(dungeonGenerator.generateDungeon());  // Dumps SVG inside DungeonGenerator
    return 0;
}
