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

Room Model::getRoom(size_t id) const
{
    return rooms_[id];
}

const Rooms& Model::getRooms() const
{
    return rooms_;
}

const Corridors& Model::getCorridors() const
{
    return corridors_;
}

size_t Model::getObjectCount() const
{
    size_t objectCount = rooms_.size();
    for (const Room& room : rooms_) {
        objectCount += room.doors.size();
    }
    return objectCount;
}

size_t Model::getVariablesCount() const
{
    return getObjectCount() * 2;
}

VariablesBounds Model::getVariablesBounds() const
{
    size_t varCount = getVariablesCount();
    VariablesBounds result(varCount);
    for (const Room& room : rooms_) {
        // Rooms don't have bounds
        const auto [roomXId, roomYId] = room.getVariablesIds();
        result[roomXId] = std::nullopt;
        result[roomYId] = std::nullopt;
        for (const Door& door : room.doors) {
            // Doors can't go beyond rooms' bounds
            const auto [doorXId, doorYId] = door.getVariablesIds();
            const double fraction = 0.3;
            result[doorXId] = Interval{.lowerBound = -room.width * fraction, .upperBound = room.width * fraction};
            result[doorYId] = Interval{.lowerBound = -room.height * fraction, .upperBound = room.height * fraction};
        }
    }
    return result;
}

void Model::setPositions(const Positions& positions)
{
    assert(positions.size() == getObjectCount() && "Model::setPositions: invalid positions count");
    for (Room& room : rooms_) {
        assert(room.id < positions.size() && "Model::setPositions: invalid room id");
        room.centerPosition = positions[room.id];
        for (Door& door : room.doors) {
            assert(door.id < positions.size() && "Model::setPositions: invalid door id");
            door.shift = positions[door.id];
        }
    }
}

void Model::dumpToSVG(const std::filesystem::path& outputPath) const
{
    std::ofstream ofstream{outputPath};
    svgw::writer svgWriter(ofstream);

    const auto [x1, y1, x2, y2] = calculateViewBox();
    const double width = x2 - x1;
    const double height = y2 - y1;
    svgWriter.start_svg(
        "100%", "100%",  // fill 100% of the screen by both axises
        {
            {"viewBox", (std::stringstream() << x1 << ' ' << y1 << ' ' << width << ' ' << height).str()}
    });
    svgWriter.write("\n");

    for (const Room& room : rooms_) {
        room.dumpToSVG(svgWriter);
        svgWriter.write("\n");
    }
    for (const Corridor& corridor : corridors_) {
        corridor.dumpToSVG(svgWriter, rooms_);
        svgWriter.write("\n");
    }

    svgWriter.end_svg();
}

std::array<double, 4> Model::calculateViewBox() const
{
    constexpr double kInf = 1e9;
    std::array<double, 4> result{kInf, kInf, -kInf, -kInf};
    for (const Room& room : rooms_) {
        assert(room.getLBPosition().has_value());
        const auto [x1, y1] = room.getLBPosition().value();
        const double x2 = x1 + room.width;
        const double y2 = y1 + room.height;
        assert(abs(x1) < kInf && abs(y1) < kInf && abs(x2) < kInf && abs(y2) < kInf);
        result[0] = std::min(result[0], x1);
        result[1] = std::min(result[1], y1);
        result[2] = std::max(result[2], x2);
        result[3] = std::max(result[3], y2);
    }
    assert(result[0] <= result[2] && result[1] <= result[3]);

    // Make padding from each side of the screen
    const double width = result[2] - result[0];
    const double height = result[3] - result[1];
    const double dx = width * 0.07;
    const double dy = height * 0.07;
    result[0] -= dx;
    result[1] -= dy;
    result[2] += dx;
    result[3] += dy;

    // Flip the Y axis
    result[1] *= -1;
    result[3] *= -1;
    std::swap(result[1], result[3]);
    assert(result[1] <= result[3]);
    return result;
}

}  // namespace Model
}  // namespace DungeonGenerator
