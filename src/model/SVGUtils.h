#pragma once

#include <string>

namespace DungeonGeneration {
namespace Model {
namespace SVGUtils {

/// Generate a code for SVG rectangle with text (which is shown on hover).
/// This is needed because text is not supported in the library used to dump SVG.
std::string generateSVGRectangle(
    double x, double y, double width, double height, const std::string& color, const std::string& text = "");

}  // namespace SVGUtils
}  // namespace Model
}  // namespace DungeonGeneration
