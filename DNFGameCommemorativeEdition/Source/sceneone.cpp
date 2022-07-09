#include "sceneone.hpp"
#include "constant.hpp"
#include "map.hpp"
#include "floorobj.hpp"

SceneOne::SceneOne(ShaderProgram* shader,
                   GLfloat frameBufferWidth,
                   GLfloat frameBufferHeight,
                   Player* player)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_player(player)
    , m_scene_one_map_boundary(glm::vec4(0.0f))
    , m_scene_one_root_node(std::make_unique<SceneNode>(StringContant::sceneOneRootNodeName))
    , m_scene_one_layer_node(new SceneNode(StringContant::sceneOneLayerNodeName))
{
    construct();
}

SceneOne::~SceneOne()
{
    // Prevent the player from being deleted.
    m_scene_one_layer_node->removeChild(m_player);
}

void
SceneOne::construct()
{
    Map* sceneOneMap = new Map(m_shader, m_frame_buffer_width, m_frame_buffer_height);
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

    for (auto floorItem : m_scene_one_map->getFloorObjs()) {
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
    for (auto const& x : m_scene_one_map->getFloorObjs()) {
        result = Utils::AABBFloorObjCollision(m_player->getPlayerFloorObjCollideGeo(),
                                              glm::vec4(x.second, x.first->getCollisionWH()),
                                              movement);

        if (result.first || result.second)
            return result;
    }

    return result;
}