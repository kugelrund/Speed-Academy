#include "landing_info.hpp"

namespace speedrun {

namespace {
LandingInfo last_info = {LandingType::None, 0};
int last_overbounce_time = 0;
}  // anonymous namespace

void SetLastLandingInfo(const LandingInfo info) {
    if (info.type == LandingType::Overbounce) {
        // Store overbounce info separately as it happens simultaneously to one
        // of the other landing types and should therefore not override but
        // amend the info
        last_overbounce_time = info.time;
        return;
    }
    last_info = info;
}

LandingInfo GetLastLandingInfo() {
    return last_info;
}

int GetLastLandingInfoOverbounceTime() {
    return last_overbounce_time;
}

}  // namespace speedrun
