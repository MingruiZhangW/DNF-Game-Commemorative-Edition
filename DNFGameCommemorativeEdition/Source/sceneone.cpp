#include "sceneone.hpp"
#include "constant.hpp"
#include "map.hpp"
#include "floorobj.hpp"

SceneOne::SceneOne(ShaderProgram* shader,
                   GLfloat frameBufferWidth,
                   GLfloat frameBufferHeight,
                   Player* player,
                   NPC* npc)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_player(player)
    , m_npc(npc)
    , m_scene_one_map_boundary(glm::vec4(0.0f))
    , m_scene_one_root_node(std::make_unique<SceneNode>(StringContant::sceneOneRootNodeName))
    , m_scene_one_layer_node(new SceneNode(StringContant::sceneOneLayerNodeName))
{
    construct();
}

SceneOne::~SceneOne()
{
    // Prevent the player and the npc from being deleted.
    m_scene_one_layer_node->removeChild(m_player);
    m_scene_one_layer_node->removeChild(m_npc);
}

void
SceneOne::construct()
{
    Map* sceneOneMap = new Map(m_shader, m_npc, m_frame_buffer_width, m_frame_buffer_height);
    sceneOneMap->initSceneOneMap();

    m_scene_one_root_node->addChild(sceneOneMap);
    m_scene_one_root_node->addChild(m_scene_one_layer_node);

    m_scene_one_map_boundary = sceneOneMap->getMapBoundary();
    m_player->setCurrentMapBoundary(m_scene_one_map_boundary);

    m_scene_one_map = sceneOneMap;

    reorderLayerNodeChild();
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