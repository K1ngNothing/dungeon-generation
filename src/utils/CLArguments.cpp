#include "CLArguments.h"

namespace DungeonGeneration {
namespace CLUtils {

void CLArgumentsHandler::setArguments(int argc, char** argv)
{
    args_.argc = argc;
    args_.argv = argv;
    initialized = true;
}

std::optional<CLArguments> CLArgumentsHandler::provideArguments() const
{
    if (initialized) {
        return args_;
    }
    return std::nullopt;
}

CLArgumentsHandler& provideCLArgumentsHandler()
{
    static CLArgumentsHandler clArgsHandler;
    return clArgsHandler;
}

}  // namespace CLUtils
}  // namespace DungeonGeneration
