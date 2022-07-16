#include "scenetwo.hpp"
#include "constant.hpp"
#include "map.hpp"
#include "floorobj.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "dialogscenenode.hpp"
#include "game.hpp"

// Offset
const static float m_player_scene_two_initial_y {175.0f};
const static float m_player_scene_two_initial_x {100.0f};

SceneTwo::SceneTwo(ShaderProgram* shader,
                   GLfloat frameBufferWidth,
                   GLfloat frameBufferHeight,
                   Player* player,
                   DialogSceneNode* dialogSceneNode)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_player(player)
    , m_monster_nums(1)
    , m_dialog_scene_node(dialogSceneNode)
    , m_current_dialog_mode(DialogConvMode::ConvOne)
    , m_scene_two_map_boundary(glm::vec4(0.0f))
    , m_scene_two_root_node(std::make_unique<SceneNode>(StringContant::sceneTwoRootNodeName))
    , m_scene_two_layer_node(new SceneNode(StringContant::sceneTwoLayerNodeName))
{
    construct();
}

SceneTwo::~SceneTwo()
{
    // Prevent the const per scene node from being deleted.
    m_scene_two_layer_node->removeChild(m_player);
    if (m_dialog_scene_node->getShown())
        m_scene_two_layer_node->removeChild(m_dialog_scene_node->getRoot());
}

void
SceneTwo::construct()
{
    Map* sceneTwoMap = new Map(m_shader, nullptr, m_frame_buffer_width, m_frame_buffer_height);
    sceneTwoMap->initSceneTwoMap();

    for (int i = 0; i < m_monster_nums; i++) {
        m_monsters.push_back(new Monster(m_shader));
    }

    m_scene_two_map_boundary = sceneTwoMap->getMapBoundary();
    m_scene_two_map = sceneTwoMap;
}

void
SceneTwo::prepareInitialDisplay()
{
    Game::enableKeyBoardEvent(false);
    m_dialog_scene_node->clearMovement();
    m_dialog_scene_node->setCurrentDialogSpeaker(StringContant::fancyPlayerChineseName);
    m_dialog_scene_node->setCurrentDialogText(Conversation::sceneTwoS1);
    m_dialog_scene_node->showDialogImage(false);
    m_dialog_scene_node->setShown(true);
    m_player->setCurrentMapBoundary(m_scene_two_map_boundary);
    m_player->translate(glm::vec3(m_player_scene_two_initial_x, m_player_scene_two_initial_y, 0.0f));
    m_monsters[0]->setCurrentMapBoundary(m_scene_two_map_boundary);
    m_monsters[0]->translate(
        glm::vec3(m_player_scene_two_initial_x + 300.0f, m_player_scene_two_initial_y, 0.0f));

    m_scene_two_root_node->addChild(m_scene_two_map);
    m_scene_two_root_node->addChild(m_scene_two_layer_node);

    reorderLayerNodeChild();
}

void
SceneTwo::checkToRemoveMonster()
{
    m_monsters.erase(std::remove_if(m_monsters.begin(),
                                    m_monsters.end(),
                                    [this](Monster* monster) {
                                        if (monster->getMonsterMode()
                                            == Monster::MonsterMode::ToBeDeleted) {
                                            m_scene_two_layer_node->removeChild(monster);
                                            return true;
                                        }
                                        return false;
                                    }),
                     m_monsters.end());
}

void
SceneTwo::reorderLayerNodeChild()
{
    // the draw order will depend on -y coord
    m_scene_two_layer_node->cleanChild();

    m_scene_two_layer_node->addChild(m_player);
    for (auto floorItem : m_scene_two_map->getFloorReorderObjs()) {
        if (floorItem.second.y > m_player->getPlayerDy() - m_player->getPlayerCenter().y) {
            m_scene_two_layer_node->addChildFront(floorItem.first);
        } else {
            m_scene_two_layer_node->addChild(floorItem.first);
        }
    }

    for (auto monster : m_monsters) {
        if (monster->getMonsterDy() > m_player->getPlayerDy() - m_player->getPlayerCenter().y) {
            m_scene_two_layer_node->addChildFront(monster);
        } else {
            m_scene_two_layer_node->addChild(monster);
        }
    }

    if (m_dialog_scene_node->getShown())
        m_scene_two_layer_node->addChild(m_dialog_scene_node->getRoot());
}

std::pair<bool, bool>
SceneTwo::sceneTwoCollisionTest(const glm::vec2& movement)
{
    std::pair<bool, bool> result {false, false};

    // Floor obj
    for (auto const& x : m_scene_two_map->getFloorCollisionObjs()) {
        result = Utils::AABBCollision(m_player->getPlayerFloorObjCollideGeo(),
                                      glm::vec4(x.second, x.first->getCollisionWH()),
                                      movement);

        if (result.first || result.second)
            return result;
    }

    return result;
}

std::pair<Monster*, bool>
SceneTwo::sceneTwoAttackCollisionTest()
{
    std::pair<bool, bool> result {false, false};

    // Monster
    for (auto monster : m_monsters) {
        result = Utils::AABBCollision(m_player->getPlayerAttackCollideGeo(),
                                      monster->getMonsterFloorObjCollideGeo(),
                                      glm::vec2(0.0f));

        if (result.first || result.second) {
            return std::make_pair(monster, true);
        }
    }

    return std::make_pair(nullptr, false);
}

void
SceneTwo::moveDialog(float dx)
{
    m_dialog_scene_node->moveDialog(glm::vec3(dx, 0.0f, 0.0f));
}

bool
SceneTwo::processClick()
{
    switch (m_current_dialog_mode) {
    case DialogConvMode::ConvOne:
        m_current_dialog_mode = DialogConvMode::ConvTwo;

        Game::enableKeyBoardEvent(true);
        m_dialog_scene_node->setShown(false);

        reorderLayerNodeChild();
        return true;
    default:
        break;
    }

    return false;
}