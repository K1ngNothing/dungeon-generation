#pragma once

#include <model/Model.h>

namespace DungeonGeneration {
namespace Callbacks {

class PushForce {
    struct RoomPair {
        size_t roomId1;
        size_t roomId2;
    };
    using RoomPairs = std::vector<RoomPair>;

public:
    PushForce(const Model::Model& model, double scale = 1.0, double range = 1.0);
    void operator()(const double* x, double& f, double* grad) const;

private:
    RoomPairs findDisconnectedRooms(const Model::Model& model) const;
    void calculatePush(RoomPair rooms, const double* x, double& f, double* grad) const;

    static constexpr bool kPushOnlyDisconnected = true;  // TODO: maybe should be moved to Settings.h

    const Model::Model& model_;
    const double scale_ = 1.0;  // the maximum value of the function
    const double range_ = 1.0;  // coefficient that determines the range where function is getting halved
    RoomPairs disconnectedRooms_;
};

}  // namespace Callbacks
}  // namespace DungeonGeneration
