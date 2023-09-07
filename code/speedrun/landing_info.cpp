#include "landing_info.hpp"

namespace speedrun {

namespace {
LandingInfo last_info = {LandingType::None, 0};
}  // anonymous namespace

void SetLastLandingInfo(const LandingInfo info) {
    last_info = info;
}

LandingInfo GetLastLandingInfo() {
    return last_info;
}

}  // namespace speedrun
