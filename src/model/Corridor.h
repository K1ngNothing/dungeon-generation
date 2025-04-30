#pragma once

#include <vector>

#include <svgwrite/writer.hpp>

#include "Door.h"
#include "Room.h"

namespace DungeonGeneration {
namespace Model {

struct Corridor {
    Door& door1;
    Door& door2;

    void dumpToSVG(svgw::writer& svgWriter, const Model::Rooms& rooms) const;
};
using Corridors = std::vector<Corridor>;

}  // namespace Model
}  // namespace DungeonGeneration
