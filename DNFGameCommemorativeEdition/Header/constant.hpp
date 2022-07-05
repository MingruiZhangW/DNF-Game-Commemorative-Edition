#pragma once

#include <string>

namespace IconPath {
const std::string iconPath {"Resource/Icon/icon.jfif"};
} // namespace IconPath

namespace StringContant {
const std::string playerName {"player_fire"};
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
const std::string playerStandPNGPath {"Resource/Texture/Player/Stand/stand.png"};
const std::string playerWalkPNGPath {"Resource/Texture/Player/Walk/walk_spritesheet.png"};
const std::string playerWalkJsonPath {"Resource/Texture/Player/Walk/walk_spritesheet.json"};
} // namespace TexturePath

namespace SpriteSize {
const float playerHeight {200.0f};
const float playerWidth {165.0f};
} // namespace SpriteSize

namespace SSJsonKeys {
const std::string frames {"frames"};
const std::string frame {"frame"};
const std::string x {"x"};
const std::string y {"y"};
const std::string w {"w"};
const std::string h {"h"};
} // namespace SSJsonKeys

namespace GLFWArrowKeyRemap {
const unsigned int upKey {15};
const unsigned int downKey {28};
const unsigned int leftKey {39};
const unsigned int rightKey {42};
} // namespace GLFWArrowKeyRemap