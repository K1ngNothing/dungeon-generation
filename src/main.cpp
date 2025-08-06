#include <filesystem>

#include <DungeonGenerator.h>
#include <utils/CLArguments.h>

using namespace DungeonGeneration;

static std::filesystem::path kPathToSVG
{
#if defined(PATH_TO_SVG)
    PATH_TO_SVG
#endif
};

int main(int argc, char** argv)
{
    CLUtils::provideCLArgumentsHandler().setArguments(argc, argv);
    DungeonGenerator dungeonGenerator;
    Model::Model model = dungeonGenerator.generateDungeon();
    model.dumpToSVG(kPathToSVG / "result.svg");
    return 0;
}
