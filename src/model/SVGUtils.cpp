#include "SVGUtils.h"

#include <sstream>

namespace DungeonGeneration {
namespace Model {
namespace SVGUtils {

std::string generateSVGRectangle(
    double x, double y, double width, double height, const std::string& color, const std::string& text)
{
    y = -y - height;  // invert Y axis; note that LB position is now different
    std::stringstream sstream;
    sstream << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width << "\" height=\"" << height << "\" fill=\""
            << color << "\" stroke=\"black\" stroke-width=\"0.5\">";
    if (!text.empty()) {
        sstream << "<title>" << text << "</title>";
    }
    sstream << "</rect>";
    return sstream.str();
}

}  // namespace SVGUtils
}  // namespace Model
}  // namespace DungeonGeneration
