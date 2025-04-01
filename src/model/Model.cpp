#include "Model.h"

#include <cassert>
#include <fstream>

#include <svgwrite/writer.hpp>

namespace DungeonGenerator {
namespace Model {

Model::Model(Rooms&& rooms, Corridors&& corridors)
      : rooms_(std::move(rooms)),
        corridors_(std::move(corridors))
{}

const Rooms& Model::getRooms() const
{
    return rooms_;
}

const Corridors& Model::getCorridors() const
{
    return corridors_;
}

void Model::setPositions(const Positions& roomPositions)
{
    assert(roomPositions.size() == rooms_.size() && "Model::setPositions: invalid positions");
    for (size_t i = 0; i < rooms_.size(); ++i) {
        assert(rooms_[i].id < roomPositions.size() && "Model::setPositions: invalid positions");
        rooms_[i].centerPosition = roomPositions[rooms_[i].id];
    }
}

void Model::dumpToSVG(const Positions& roomPositions, const std::filesystem::path& outputPath) const
{
    std::ofstream ofstream{outputPath};
    svgw::writer svgWriter(ofstream);

    svgWriter.start_svg(
        800, 800,
        {
            {"viewBox", "-400 -400 800 800"}
    });
    svgWriter.write("\n");

    for (const Room& room : rooms_) {
        room.dumpToSVG(svgWriter);
        svgWriter.write("\n");
    }
    for (const Corridor& corridor : corridors_) {
        corridor.dumpToSVG(svgWriter);
        svgWriter.write("\n");
    }

    svgWriter.end_svg();
}
}  // namespace Model
}  // namespace DungeonGenerator
