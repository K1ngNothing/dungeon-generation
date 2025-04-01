#include "SVGUtils.h"

#include <sstream>

namespace DungeonGenerator {
namespace Model {
namespace SVGUtils {

std::string getSVGRectangle(
    double x, double y, double width, double height, const std::string& color, const std::string& text)
{
    y = -y - height;  // invert Y axis; note that LB position is now different
    std::stringstream sstream;
    sstream << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width << "\" height=\"" << height << "\" fill=\""
            << color << "\">";
    if (!text.empty()) {
        sstream << "<title>" << text << "</title>";
    }
    sstream << "</rect>";
    return sstream.str();
}

}  // namespace SVGUtils
}  // namespace Model
}  // namespace DungeonGenerator
