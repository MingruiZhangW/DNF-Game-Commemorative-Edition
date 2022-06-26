#pragma once

#include <string>

namespace StringContant {
const std::string sceneOneRootNodeName {"root_scene_one"};
const std::string sceneOneBackgroundName {"scene_one_background"};
const std::string sceneOneFloorName {"scene_one_floor"};
} // namespace StringContant

namespace RatioContant {
const float backgroundHeightScaleRatio {4.0f / 7.0f};
const float floorHeightScaleRatio {3.0f / 7.0f + 0.1f};
} // namespace RatioContant

namespace FPS {
// FPS
const float fps {60.0f};
// One frame takes how many milliseconds
const float frameDelay {1000.0f / fps};
} // namespace FPS