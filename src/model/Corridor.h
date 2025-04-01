#pragma once

#include <vector>

#include <svgwrite/writer.hpp>

#include "Door.h"

namespace DungeonGenerator {
namespace Model {

struct Corridor {
    Door& door1;
    Door& door2;

    void dumpToSVG(svgw::writer& svgWriter) const;
};
using Corridors = std::vector<Corridor>;

}  // namespace Model
}  // namespace DungeonGenerator
