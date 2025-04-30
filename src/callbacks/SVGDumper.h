#pragma once

#include <filesystem>

#include <model/Model.h>

namespace DungeonGeneration {
namespace Callbacks {

class SVGDumper {
public:
    SVGDumper(Model::Model& model, const std::filesystem::path& pathToSVG, const std::string& filenamePrefix);
    void operator()(const double* x, int iterNum);

private:
    Model::Model& model_;
    std::filesystem::path pathToSVG_;
    std::string filenamePrefix_;
};

}  // namespace Callbacks
}  // namespace DungeonGeneration
