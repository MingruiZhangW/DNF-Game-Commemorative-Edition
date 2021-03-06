#pragma once

#include <string>
#include <limits>
#include <cmath>
#include <random>

#include <glm.hpp>
#include <gtx/string_cast.hpp>
#include <limits>

namespace Utils {
static bool
areSame(float a, float b)
{
    return fabs(a - b) < std::numeric_limits<float>::epsilon();
}

static float
randomBetweenZeroAndOne()
{
    return static_cast<float>(std::rand() % 101) / 100.0f;
}

static int
randomBetween(int low, int high)
{
    return rand() % (high - low + 1) + low;
}

static std::pair<bool, bool>
AABBCollision(const glm::vec4& vecA, const glm::vec4& vecB, const glm::vec2& movement)
{
    // x, y -> x, y, x,y at bottom - left corner
    // z -> width
    // w -> height
    // we want the feet of the player to collide with the bottom of the object.
    // the bounding box of the object B and A will not be the texture or the plane size.
    // return which direction (horizontal or vertical) are collided
    if ((vecA.x + vecA.z >= vecB.x) && (vecB.x + vecB.z >= vecA.x) && (vecA.y + vecA.w >= vecB.y)
        && (vecB.y + vecB.w >= vecA.y)) {
        if (movement.x == 0.0f && movement.y == 0.0f)
            return std::make_pair(true, true);

        float dxEntry, dxExit;
        float dyEntry, dyExit;
        float txEntry, txExit;
        float tyEntry, tyExit;
        if (movement.x > 0.0f) {
            dxEntry = vecB.x - (vecA.x + vecA.z);
            dxExit = (vecB.x + vecB.z) - vecA.x;
        } else {
            dxEntry = (vecB.x + vecB.z) - vecA.x;
            dxExit = vecB.x - (vecA.x + vecA.z);
        }
        if (movement.y > 0.0f) {
            dyEntry = vecB.y - (vecA.y + vecA.w);
            dyExit = (vecB.y + vecB.w) - vecA.y;

        } else {
            dyEntry = (vecB.y + vecB.w) - vecA.y;
            dyExit = vecB.y - (vecA.y + vecA.w);
        }

        if (movement.x == 0.0f) {
            txEntry = -std::numeric_limits<float>::infinity();
            txExit = std::numeric_limits<float>::infinity();
        } else {
            txEntry = dxEntry / movement.x;
            txExit = dyExit / movement.x;
        }

        if (movement.y == 0.0f) {
            tyEntry = -std::numeric_limits<float>::infinity();
            tyExit = std::numeric_limits<float>::infinity();
        } else {
            tyEntry = dyEntry / movement.y;
            tyExit = dyExit / movement.y;
        }

        // horizontal, vertical
        return std::make_pair(txEntry > tyEntry, txEntry <= tyEntry);
    }

    return std::make_pair(false, false);
}
} // namespace Utils

namespace IconPath {
const std::string iconPath {"Resource/Icon/icon.jfif"};
} // namespace IconPath

namespace CursorPath {
const std::string cursorPath {"Resource/Texture/Cursor/cursor.png"};
} // namespace CursorPath

namespace StringContant {
// Wide character string is used here for chinese names
const std::wstring fancyPlayerChineseName {L"????????"};
const std::wstring fancyNPCChineseName {L"??????????"};
const std::string npcName {"npc_linus"};
const std::string playerName {"player_fire"};
const std::string playerSkillEffectName {"player_effect"};
const std::string monsterName {"monster_goblin"};
const std::string dialogUIName {"dialog_ui"};
const std::string dialogUIImageName {"dialog_ui_image"};
const std::string dialogRootNodeName {"root_dialog"};
const std::string dialogCharNodeName {"dialog_char"};
const std::string sceneZeroRootNodeName {"root_scene_zero"};
const std::string sceneZeroBackgroundName {"scene_zero_background"};
const std::string sceneOneRootNodeName {"root_scene_one"};
const std::string sceneTwoRootNodeName {"root_scene_two"};
const std::string sceneOneLayerNodeName {"scene_one_layer_node"};
const std::string sceneTwoLayerNodeName {"scene_two_layer_node"};
const std::string sceneOneBackgroundName {"scene_one_background"};
const std::string sceneTwoBackgroundName {"scene_two_background"};
const std::string sceneOneFloorName {"scene_one_floor"};
const std::string sceneTwoFloorName {"scene_two_floor"};
const std::string sceneOneMapName {"scene_one_map"};
const std::string sceneTwoMapName {"scene_two_map"};
const std::string sceneOneFloorObjName {"scene_one_floor_obj"};
const std::string sceneTwoFloorObjName {"scene_two_floor_obj"};
const std::string buttonName {"button"};
const std::string starParticlesGeneratorName {"star_particles_generator"};
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

namespace Scene {
enum class SceneEvents { None, QuitGame, SceneTransit, DialogClick };
} // namespace Scene

namespace Fonts {
struct CharacterInfo
{
    GLuint textureID;     // Texture ID
    GLuint textureWidth;  // Width of glyph texture
    GLuint textureHeight; // Height of glyph texture
    int bearingX;         // Offset from baseline to left of glyph (only used in english)
    int bearingY;         // Offset from baseline to top of glyph (only used in english)
    long advance;         // Offset to advance to next glyph (only used in english)
};

const int fontTextSize {24};
const float fontNameScale {24.0f};
const float fontTextScale {24.0f};
const float fontNameAdvance {3.0f};
const float fontTextLineWidth {500.0f};
const float fontTextLineHeight {27.0f};

const std::string chineseFontPath {"Resource/Fonts/FZFangSong-Z02T-Regular.ttf"};
const std::string englishFontPath {"Resource/Fonts/ARLRDBD.ttf"};
// Wide character string is used here for chinese names
const std::wstring chineseLetters {L"??????????????????"};
const std::string englishLetters {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,.! ?"};
} // namespace Fonts

namespace TexturePath {
const std::string dnfLogoPath {"Resource/Texture/UI/dnfLogo.png"};
const std::string mouseLeftPath {"Resource/Texture/UI/mouseLeft.png"};
const std::string xKeyPath {"Resource/Texture/UI/xKey.png"};
const std::string zKeyPath {"Resource/Texture/UI/zKey.png"};
const std::string arrowKeyPath {"Resource/Texture/UI/arrowKey.png"};
const std::string moveLabelPath {"Resource/Texture/UI/moveLabel.png"};
const std::string attackLabelPath {"Resource/Texture/UI/attackLabel.png"};
const std::string interactLabelPath {"Resource/Texture/UI/interactLabel.png"};
const std::string exitButtonNormalUIPath {"Resource/Texture/UI/exitButtonNormal.png"};
const std::string exitButtonHoverUIPath {"Resource/Texture/UI/exitButtonHover.png"};
const std::string controlButtonNormalUIPath {"Resource/Texture/UI/controlButtonNormal.png"};
const std::string controlButtonHoverUIPath {"Resource/Texture/UI/controlButtonHover.png"};
const std::string backButtonNormalUIPath {"Resource/Texture/UI/backButtonNormal.png"};
const std::string backButtonHoverUIPath {"Resource/Texture/UI/backButtonHover.png"};
const std::string playButtonNormalUIPath {"Resource/Texture/UI/playButtonNormal.png"};
const std::string playButtonHoverUIPath {"Resource/Texture/UI/playButtonHover.png"};
const std::string starPNGPath {"Resource/Texture/Particles/star.png"};
const std::string victoryPNGPath {"Resource/Texture/UI/victory.png"};
const std::string defeatPNGPath {"Resource/Texture/UI/defeat.png"};
const std::string dialogUIPath {"Resource/Texture/UI/dialog.png"};
const std::string npcDialogHeadPath {"Resource/Texture/NPC/npcDialogHead.png"};
const std::string doorBushPath {"Resource/Texture/Door/doorBush.png"};
const std::string sideDoorNormalPath {"Resource/Texture/Door/sideNormalDoor.png"};
const std::string sideDoorBossPath {"Resource/Texture/Door/sideBossDoor.png"};
const std::string treePath {"Resource/Texture/FloorObj/tree.png"};
const std::string flowerOnePath {"Resource/Texture/FloorObj/flowerOne.png"};
const std::string flowerTwoPath {"Resource/Texture/FloorObj/flowerTwo.png"};
const std::string backgroundFarPath {"Resource/Texture/Background/backgroundFar.png"};
const std::string backgoundMidPath {"Resource/Texture/Background/backgoundMid.png"};
const std::string sceneZeroBackgroundPath {"Resource/Texture/Background/sceneZeroBackground.png"};
const std::string grassTilePath {"Resource/Texture/Tiles/grassTile.png"};
const std::string roadTilePath {"Resource/Texture/Tiles/roadTile.png"};
const std::string npcPNGPath {"Resource/Texture/NPC/npc_spritesheet.png"};
const std::string npcJsonPath {"Resource/Texture/NPC/npc_spritesheet.json"};
const std::string playerStandPNGPath {"Resource/Texture/Player/Stand/stand_spritesheet.png"};
const std::string playerStandJsonPath {"Resource/Texture/Player/Stand/stand_spritesheet.json"};
const std::string playerWalkPNGPath {"Resource/Texture/Player/Walk/walk_spritesheet.png"};
const std::string playerWalkJsonPath {"Resource/Texture/Player/Walk/walk_spritesheet.json"};
const std::string playerBasicAttackPNGPath {
    "Resource/Texture/Player/BasicAttack/basicattack_spritesheet.png"};
const std::string playerBasicAttackJsonPath {
    "Resource/Texture/Player/BasicAttack/basicattack_spritesheet.json"};
const std::string playerSkillPNGPath {"Resource/Texture/Player/Skill/skill_spritesheet.png"};
const std::string playerSKillJsonPath {"Resource/Texture/Player/Skill/skill_spritesheet.json"};
const std::string playerSKillEffectPNGPath {
    "Resource/Texture/Player/Skill/skill_effect_spritesheet.png"};
const std::string playerSKillEffectJsonPath {
    "Resource/Texture/Player/Skill/skill_effect_spritesheet.json"};
const std::string monsterStandPNGPath {"Resource/Texture/Monster/Stand/monster_spritesheet.png"};
const std::string monsterStandJsonPath {"Resource/Texture/Monster/Stand/monster_spritesheet.json"};
const std::string monsterWalkPNGPath {"Resource/Texture/Monster/Walk/walk_spritesheet.png"};
const std::string monsterWalkJsonPath {"Resource/Texture/Monster/Walk/walk_spritesheet.json"};
const std::string monsterKilledPNGPath {"Resource/Texture/Monster/Killed/killed_spritesheet.png"};
const std::string monsterKilledJsonPath {"Resource/Texture/Monster/Killed/killed_spritesheet.json"};
} // namespace TexturePath

namespace ShadowShaderPath {
const std::string vertexShader {"Resource/Shaders/shadow_shader.vert"};
const std::string fragmentShader {"Resource/Shaders/shadow_shader.frag"};
} // namespace ShadowShaderPath

namespace ParticleShaderPath {
const std::string vertexShader {"Resource/Shaders/particles_shader.vert"};
const std::string fragmentShader {"Resource/Shaders/particles_shader.frag"};
} // namespace ParticleShaderPath

namespace Conversation {
const std::string sceneOneS1 {"Where.....am I ? ? ?"};
const std::string sceneOneS2 {"I...should be at the Kether."};
const std::string sceneOneS3 {"My head....I need to check this place around..."};
const std::string sceneOneS4 {"Do you know where is this place ? Human."};
const std::string sceneOneS5 {"Yeah, Human hmm... Here is Lorraine. "
                              + std::string("If you want to know more...Show me your strength ! ")
                              + "There are a bunch of monsters ahead. Kill them !"};
const std::string sceneOneS6 {"Well. Consider it as a gift... Human."};
const std::string sceneOneClick {"Kill these dangerous and aggressive monsters !"};
const std::string sceneTwoS1 {"Goblins... This will be fast."};
const std::string sceneTwoS2 {"Time to head back."};
} // namespace Conversation

namespace DialogSize {
const float dialogNameXOffset {230.0f};
const float dialogTextXOffset {230.0f};
const float dialogNameYOffset {-10.0f};
const float dialogTextYOffset {-20.0f - Fonts::fontNameScale};
const float dialogWidthRatio {2.0f / 3.0f};
const float dialogHeight {200.0f};
const float imageHeight {222.0f};
const float imageWidth {185.0f};
} // namespace DialogSize

namespace SpriteSize {
const float npcHeight {278.0f};
const float npcWidth {150.0f};
const float playerHeight {165.0f};
const float playerWidth {200.0f};
const float monsterHeight {165.0f};
const float monsterWidth {101.0f};
} // namespace SpriteSize

namespace CollisionOffset {
const float horizontalCollisionOffset {2.5f};
const float verticalCollisionOffset {2.5f};
} // namespace CollisionOffset

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

namespace PlayerFollowCameraSpeed {
const float walk {10.0f};
const float run {20.0f};
} // namespace PlayerFollowCameraSpeed

namespace SoundPath {
const std::string sceneZeroBg {"Resource/Sound/bgmSceneZero.ogg"};
const std::string buttonHover {"Resource/Sound/buttonHover.ogg"};
const std::string buttonClick {"Resource/Sound/buttonClick.ogg"};
const std::string sceneOneTwoBg {"Resource/Sound/overThink.ogg"};
const std::string monsterBeHit {"Resource/Sound/monsterBeHitSound.ogg"};
const std::string cut {"Resource/Sound/cutSound.ogg"};
const std::string playerAttack {"Resource/Sound/playerAttack.ogg"};
} // namespace SoundPath