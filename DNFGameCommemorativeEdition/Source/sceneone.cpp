#include "sceneone.h"
#include "constant.hpp"
#include "map.hpp"

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
{
    construct();
}

SceneOne::~SceneOne()
{
    // Prevent the player from being deleted.
    m_scene_one_root_node->removeChild(m_player);
}

void
SceneOne::construct()
{
    Map* sceneOneMap = new Map(m_shader, m_frame_buffer_width, m_frame_buffer_height);
    sceneOneMap->initSceneOneMap();

    m_scene_one_root_node->addChild(sceneOneMap);
    m_scene_one_root_node->addChild(m_player);

    m_scene_one_map_boundary = sceneOneMap->getMapBoundary();
    m_player->setCurrentMapBoundary(m_scene_one_map_boundary);
}