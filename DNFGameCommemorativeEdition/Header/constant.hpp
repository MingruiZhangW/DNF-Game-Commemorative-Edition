#pragma once

#include <string>

namespace StringContant {
const std::string sceneOneRootNodeName {"root_scene_one"};
const std::string sceneOneBackgroundName {"scene_one_background"};
const std::string sceneOneFloorName {"scene_one_floor"};
const std::string sceneOneMapName {"scene_one_map"};
} // namespace StringContant

namespace RatioContant {
const float backgroundHeightScaleRatio {5.0f / 8.0f};
const float floorHeightScaleRatio {3.5f / 8.0f};
const float attachOffset {1.1f};
} // namespace RatioContant

namespace FPS {
// FPS
const float fps {60.0f};
// One frame takes how many milliseconds
const float frameDelay {1000.0f / fps};
} // namespace FPS

namespace TexturePath {
const std::string backgroundFarPath {"Resource/Texture/Background/backgroundFar.png"};
const std::string backgoundMidPath {"Resource/Texture/Background/backgoundMid.png"};
const std::string grassTilePath {"Resource/Texture/Tiles/grassTile.png"};
const std::string roadTilePath {"Resource/Texture/Tiles/roadTile.png"};
} // namespace TexturePath