#pragma once

namespace speedrun {

enum class LandingType {
    None = -1,
    CrouchBoost,
    ElevationBoost,
    RandomBoost,
    SpinGlitch,
    VelocityBoost,
    VRGI,

    NumLandingTypes,
};

inline constexpr int numLandingTypes() {
    return static_cast<int>(LandingType::NumLandingTypes);
}

inline int toInt(const LandingType type) {
    return static_cast<int>(type);
}

struct LandingInfo {
    LandingType type;
    int time;
};

void SetLastLandingInfo(const LandingInfo info);
LandingInfo GetLastLandingInfo();

}  // namespace speedrun
