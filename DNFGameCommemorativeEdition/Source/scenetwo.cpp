#include "scenetwo.hpp"
#include "map.hpp"
#include "floorobj.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "dialogscenenode.hpp"
#include "game.hpp"
#include "button.hpp"
#include "flockingengine.hpp"

// Const and offset
const static float m_player_scene_two_initial_y {175.0f};
const static float m_player_scene_two_initial_x {100.0f};

const static float m_exit_button_y_offset {-225.0f};

const static unsigned int m_monster_max_num {5};

SceneTwo::SceneTwo(ShaderProgram* shader,
                   GLfloat frameBufferWidth,
                   GLfloat frameBufferHeight,
                   Player* player,
                   DialogSceneNode* dialogSceneNode,
                   StarParticlesGenerator* starParticlesGenerator)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_player(player)
    , m_monster_nums(m_monster_max_num)
    , m_star_particles_generator(starParticlesGenerator)
    , m_dialog_scene_node(dialogSceneNode)
    , m_current_stage(SceneTwoStage::ConvOne)
    , m_scene_dx(0.0f)
    , m_hover_changed(false)
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
    m_scene_two_layer_node->removeChild(m_star_particles_generator);
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

    m_exit_button = new Button(StringContant::buttonName + "exit",
                               m_shader,
                               Button::ButtonTextureType::ExitButton);

    m_victory_logo = new Button(StringContant::buttonName + "victory",
                                m_shader,
                                Button::ButtonTextureType::Victory);
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

    for (size_t i = 0; i < m_monsters.size(); i++) {
        m_monsters[i]->setCurrentMapBoundary(glm::vec4(m_scene_two_map_boundary.x,
                                                       m_scene_two_map_boundary.y,
                                                       m_scene_two_map_boundary.z + 100.0f,
                                                       m_scene_two_map_boundary.w - 100.0f));

        // Assign pos to each monster in a manual way
        switch (i) {
        case 0: {
            m_monsters[i]->translate(glm::vec3(m_player_scene_two_initial_x + 500.0f,
                                               m_player_scene_two_initial_y - 100.0f,
                                               0.0f));
            break;
        }
        case 1: {
            m_monsters[i]->translate(glm::vec3(m_player_scene_two_initial_x + 300.0f,
                                               m_player_scene_two_initial_y,
                                               0.0f));
            break;
        }
        case 2: {
            m_monsters[i]->translate(glm::vec3(m_player_scene_two_initial_x + 600.0f,
                                               m_player_scene_two_initial_y + 100.0f,
                                               0.0f));
            break;
        }
        case 3: {
            m_monsters[i]->translate(glm::vec3(m_player_scene_two_initial_x + 400.0f,
                                               m_player_scene_two_initial_y + 150.0f,
                                               0.0f));
            break;
        }
        case 4: {
            m_monsters[i]->translate(glm::vec3(m_player_scene_two_initial_x + 700.0f,
                                               m_player_scene_two_initial_y - 200.0f,
                                               0.0f));
            break;
        }
        default:
            break;
        }

        // Give fake initial pos for flocking
        m_monsters[i]->setLastMonsterTrans(
            glm::vec3(Utils::randomBetween(-10, 10), Utils::randomBetween(-10, 10), 0.0f));
    }

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

    if (m_monsters.size() == 0 && m_current_stage == SceneTwoStage::Fighting) {
        m_current_stage = SceneTwoStage::ConvTwo;

        Game::enableKeyBoardEvent(false);
        m_dialog_scene_node->setCurrentDialogSpeaker(StringContant::fancyPlayerChineseName);
        m_dialog_scene_node->setCurrentDialogText(Conversation::sceneTwoS2);
        m_dialog_scene_node->showDialogImage(false);
        m_dialog_scene_node->setShown(true);

        reorderLayerNodeChild();
    }
}

void
SceneTwo::updateMonsterFlockingMovements()
{
    if (Game::getEnableKeyBoardEvent()) {
        FlockingEngine::updatePosition(m_monsters);

        reorderLayerNodeChild();
    }
}
#pragma optimize("", off)
void
SceneTwo::reorderLayerNodeChild()
{
    // the draw order will depend on -y coord
    m_scene_two_layer_node->cleanChild();

    int monsterIndex {0};
    int floorObjIndex {0};
    bool playerAdded {false};
    auto floorObjs = m_scene_two_map->getFloorReorderObjs();

    // Sort Monster, floor obj is sorted
    std::sort(m_monsters.begin(), m_monsters.end(), [](Monster* monsterA, Monster* monsterB) {
        return monsterA->getMonsterDy() > monsterB->getMonsterDy();
    });

    // Sort floor obj
    std::sort(floorObjs.begin(),
              floorObjs.end(),
              [](std::pair<FloorObj*, glm::vec2> pairA, std::pair<FloorObj*, glm::vec2> pairB) {
                  return pairA.second.y > pairB.second.y;
              });

    // Rearrange
    for (size_t i = 0; i < m_monsters.size() + floorObjs.size() + 1; i++) {
        if (monsterIndex == m_monsters.size()
            || floorObjs[floorObjIndex].second.y
                   > (m_monsters[monsterIndex]->getMonsterDy()
                      - m_monsters[monsterIndex]->getMonsterCenter().y)) {
            if (!playerAdded) {
                if (floorObjs[floorObjIndex].second.y
                    < (m_player->getPlayerDy() - m_player->getPlayerCenter().y)) {
                    m_scene_two_layer_node->addChild(m_player);

                    playerAdded = true;

                    continue;
                }
            }

            m_scene_two_layer_node->addChild(floorObjs[floorObjIndex].first);
            floorObjIndex++;
        } else {
            if (!playerAdded) {
                if (m_monsters[monsterIndex]->getMonsterDy() < m_player->getPlayerDy()) {
                    m_scene_two_layer_node->addChild(m_player);

                    playerAdded = true;

                    continue;
                }
            }

            m_scene_two_layer_node->addChild(m_monsters[monsterIndex]);
            monsterIndex++;
        }
    }

    // Monster should face right if monster x pos is smaller than player
    for (auto& i : m_monsters) {
        if (i->getMonsterDx() < m_player->getPlayerDx()) {
            if (i->spriteIsFacingLeft())
                i->flipSprite();
        } else if (!i->spriteIsFacingLeft()) {
            i->flipSprite();
        }
    }

    // Add victory stage stuff
    if (m_current_stage == SceneTwoStage::Victory) {
        m_star_particles_generator->translate(
            glm::vec3(m_scene_dx + m_frame_buffer_width / 2.0f, m_frame_buffer_height / 2.0f, 0.0f));
        m_victory_logo->translateToWindowCenter(m_scene_dx,
                                                m_frame_buffer_width,
                                                m_frame_buffer_height);
        m_exit_button->translateToWindowCenter(m_scene_dx,
                                               m_frame_buffer_width,
                                               m_frame_buffer_height);
        m_exit_button->translate(glm::vec3(0.0f, m_exit_button_y_offset, 0.0f));

        m_scene_two_layer_node->addChild(m_star_particles_generator);
        m_scene_two_layer_node->addChild(m_victory_logo);
        m_scene_two_layer_node->addChild(m_exit_button);
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

std::vector<std::pair<Monster*, bool>>
SceneTwo::sceneTwoAttackCollisionTest()
{
    std::vector<std::pair<Monster*, bool>> result {};

    // Monster
    for (auto monster : m_monsters) {
        auto resultForOneMonster = Utils::AABBCollision(m_player->getPlayerAttackCollideGeo(),
                                                        monster->getMonsterFloorObjCollideGeo(),
                                                        glm::vec2(0.0f));

        if (resultForOneMonster.first || resultForOneMonster.second) {
            result.push_back(std::make_pair(monster, true));
        }
    }

    return result;
}

void
SceneTwo::moveDialog(float dx)
{
    m_scene_dx = m_scene_dx + dx;
    m_dialog_scene_node->moveDialog(glm::vec3(dx, 0.0f, 0.0f));
}

bool
SceneTwo::processHover(const glm::vec2& mousePos)
{
    switch (m_current_stage) {
    case SceneTwoStage::Victory: {
        auto isHovered = m_exit_button->checkOnTop(mousePos);
        if (m_hover_changed != isHovered) {
            m_hover_changed = isHovered;
            return m_exit_button->checkOnTop();
        }
        break;
    }
    }

    return false;
}

Scene::SceneEvents
SceneTwo::processClick()
{
    switch (m_current_stage) {
    case SceneTwoStage::ConvOne:
        Game::enableKeyBoardEvent(true);
        m_dialog_scene_node->setShown(false);

        reorderLayerNodeChild();

        m_current_stage = SceneTwoStage::Fighting;

        return Scene::SceneEvents::DialogClick;
    case SceneTwoStage::ConvTwo:
        m_dialog_scene_node->setShown(false);

        m_current_stage = SceneTwoStage::Victory;

        reorderLayerNodeChild();
        return Scene::SceneEvents::DialogClick;
    case SceneTwoStage::Victory:
        if (m_exit_button->checkOnTop())
            return Scene::SceneEvents::QuitGame;
        return Scene::SceneEvents::None;
    default:
        break;
    }

    return Scene::SceneEvents::None;
}