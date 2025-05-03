#pragma once

#include <optional>

namespace DungeonGeneration {
namespace CLUtils {

struct CLArguments {
    int argc;
    char** argv;
};

class CLArgumentsHandler {
public:
    CLArgumentsHandler() = default;

    void setArguments(int argc, char** argv);
    std::optional<CLArguments> provideArguments() const;

private:
    CLArguments args_ = CLArguments{.argc = 0, .argv = nullptr};
    bool initialized = false;
};

/// Singleton
CLArgumentsHandler& provideCLArgumentsHandler();

}  // namespace CLUtils
}  // namespace DungeonGeneration
