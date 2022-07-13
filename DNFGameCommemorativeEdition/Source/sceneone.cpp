#include "sceneone.hpp"
#include "constant.hpp"
#include "map.hpp"
#include "floorobj.hpp"
#include "player.hpp"
#include "npc.hpp"
#include "dialogscenenode.hpp"
#include "game.hpp"

SceneOne::SceneOne(ShaderProgram* shader,
                   GLfloat frameBufferWidth,
                   GLfloat frameBufferHeight,
                   Player* player,
                   NPC* npc,
                   DialogSceneNode* dialogSceneNode)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_player(player)
    , m_npc(npc)
    , m_dialog_scene_node(dialogSceneNode)
    , m_current_dialog_mode(DialogConvMode::ConvOne)
    , m_scene_one_map_boundary(glm::vec4(0.0f))
    , m_scene_one_root_node(std::make_unique<SceneNode>(StringContant::sceneOneRootNodeName))
    , m_scene_one_layer_node(new SceneNode(StringContant::sceneOneLayerNodeName))
{
    construct();
}

SceneOne::~SceneOne()
{
    // Prevent the const per scene node from being deleted.
    m_scene_one_layer_node->removeChild(m_player);
    m_scene_one_layer_node->removeChild(m_npc);
    if (m_dialog_scene_node->getShown())
        m_scene_one_layer_node->removeChild(m_dialog_scene_node->getRoot());
}

void
SceneOne::construct()
{
    Map* sceneOneMap = new Map(m_shader, m_npc, m_frame_buffer_width, m_frame_buffer_height);
    sceneOneMap->initSceneOneMap();

    m_scene_one_map_boundary = sceneOneMap->getMapBoundary();
    m_player->setCurrentMapBoundary(m_scene_one_map_boundary);

    m_scene_one_map = sceneOneMap;
}

void
SceneOne::prepareInitialDisplay()
{
    Game::enableKeyBoardEvent(false);
    m_dialog_scene_node->setCurrentDialogSpeaker(StringContant::fancyPlayerChineseName);
    m_dialog_scene_node->setCurrentDialogText(Conversation::sceneOneS1);
    m_dialog_scene_node->showDialogImage(false);
    m_dialog_scene_node->setShown(true);
    m_scene_one_layer_node->addChild(m_dialog_scene_node->getRoot());

    m_scene_one_root_node->addChild(m_scene_one_layer_node);
}

void
SceneOne::reorderLayerNodeChild()
{
    // the draw order will depend on -y coord
    m_scene_one_layer_node->cleanChild();

    m_scene_one_layer_node->addChild(m_player);
    if (m_npc->getNPCDy() > m_player->getPlayerDy() - m_player->getPlayerCenter().y) {
        m_scene_one_layer_node->addChildFront(m_npc);
    } else {
        m_scene_one_layer_node->addChild(m_npc);
    }

    for (auto floorItem : m_scene_one_map->getFloorReorderObjs()) {
        if (floorItem.second.y > m_player->getPlayerDy() - m_player->getPlayerCenter().y) {
            m_scene_one_layer_node->addChildFront(floorItem.first);
        } else {
            m_scene_one_layer_node->addChild(floorItem.first);
        }
    }

    if (m_dialog_scene_node->getShown())
        m_scene_one_layer_node->addChild(m_dialog_scene_node->getRoot());
}

std::pair<bool, bool>
SceneOne::sceneOneCollisionTest(const glm::vec2& movement)
{
    std::pair<bool, bool> result {false, false};

    // NPC
    result = Utils::AABBFloorObjCollision(m_player->getPlayerFloorObjCollideGeo(),
                                          m_npc->getNPCCollideGeo(),
                                          movement);

    if (result.first || result.second)
        return result;

    // Floor obj
    for (auto const& x : m_scene_one_map->getFloorCollisionObjs()) {
        result = Utils::AABBFloorObjCollision(m_player->getPlayerFloorObjCollideGeo(),
                                              glm::vec4(x.second, x.first->getCollisionWH()),
                                              movement);

        if (result.first || result.second)
            return result;
    }

    return result;
}

void
SceneOne::moveDialog(float dx)
{
    m_dialog_scene_node->moveDialog(glm::vec3(dx, 0.0f, 0.0f));
}

void
SceneOne::processHover(const glm::vec2& mousePos)
{
    m_npc->checkOnTop(mousePos);
}

void
SceneOne::processClick()
{
    switch (m_current_dialog_mode) {
    case DialogConvMode::ConvOne:
        m_dialog_scene_node->setCurrentDialogText(Conversation::sceneOneS2);
        m_current_dialog_mode = DialogConvMode::ConvTwo;
        return;
    case DialogConvMode::ConvTwo:
        m_dialog_scene_node->setCurrentDialogText(Conversation::sceneOneS3);
        m_current_dialog_mode = DialogConvMode::ConvThree;
        return;
    case DialogConvMode::ConvThree:
        m_current_dialog_mode = DialogConvMode::ConvFour;

        Game::enableKeyBoardEvent(true);
        m_scene_one_root_node->removeChild(m_scene_one_layer_node);
        m_scene_one_root_node->addChild(m_scene_one_map);
        m_scene_one_root_node->addChild(m_scene_one_layer_node);

        m_scene_one_layer_node->removeChild(m_dialog_scene_node->getRoot());
        m_dialog_scene_node->setShown(false);

        reorderLayerNodeChild();
        break;

    case DialogConvMode::ConvFour:
        if (m_npc->checkOnTop() && !m_dialog_scene_node->getShown()) {
            m_dialog_scene_node->setCurrentDialogSpeaker(StringContant::fancyPlayerChineseName);
            m_dialog_scene_node->setCurrentDialogText(Conversation::sceneOneS4);
            m_scene_one_layer_node->addChild(m_dialog_scene_node->getRoot());
            m_dialog_scene_node->setShown(true);

            m_current_dialog_mode = DialogConvMode::ConvFive;
        }
        break;
    case DialogConvMode::ConvFive:
        m_dialog_scene_node->showDialogImage(true);
        m_dialog_scene_node->setCurrentDialogSpeaker(StringContant::fancyNPCChineseName);
        m_dialog_scene_node->setCurrentDialogText(Conversation::sceneOneS5);
        m_current_dialog_mode = DialogConvMode::ConvSix;
        break;
    case DialogConvMode::ConvSix:
        m_dialog_scene_node->showDialogImage(false);
        m_dialog_scene_node->setCurrentDialogSpeaker(StringContant::fancyPlayerChineseName);
        m_dialog_scene_node->setCurrentDialogText(Conversation::sceneOneS6);
        m_current_dialog_mode = DialogConvMode::ConvSeven;
        break;
    case DialogConvMode::ConvSeven:
        m_scene_one_layer_node->removeChild(m_dialog_scene_node->getRoot());
        m_dialog_scene_node->setShown(false);
        m_dialog_scene_node->showDialogImage(true);
        m_current_dialog_mode = DialogConvMode::ConvEight;
        break;
    case DialogConvMode::ConvEight:
        if (m_npc->checkOnTop() && !m_dialog_scene_node->getShown()) {
            m_dialog_scene_node->setCurrentDialogSpeaker(StringContant::fancyNPCChineseName);
            m_dialog_scene_node->setCurrentDialogText(Conversation::sceneOneClick);

            m_scene_one_layer_node->addChild(m_dialog_scene_node->getRoot());

            m_dialog_scene_node->setShown(true);
        } else if (m_dialog_scene_node->getShown()) {
            m_scene_one_layer_node->removeChild(m_dialog_scene_node->getRoot());

            m_dialog_scene_node->setShown(false);
        }
        break;
    }
}