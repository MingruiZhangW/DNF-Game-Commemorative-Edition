#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"

#include "GL/glew.h"

#include <memory>

class Map;
class Player;
class NPC;
class DialogSceneNode;

class SceneOne
{
public:
    SceneOne(ShaderProgram* shader,
             GLfloat frameBufferWidth,
             GLfloat frameBufferHeight,
             Player* player,
             NPC* npc,
             DialogSceneNode* dialogSceneNode);

    ~SceneOne();

    SceneNode* getRootSceneNode()
    {
        return m_scene_one_root_node.get();
    }

    glm::vec4 getSceneOneMapBoundary()
    {
        return m_scene_one_map_boundary;
    }

    std::pair<bool, bool> sceneOneCollisionTest(const glm::vec2& movement);

    void reorderLayerNodeChild();

private:
    void construct();

    ShaderProgram* m_shader;

    Player* m_player;
    NPC* m_npc;
    DialogSceneNode* m_dialog_scene_node;

    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    glm::vec4 m_scene_one_map_boundary;

    std::unique_ptr<SceneNode> m_scene_one_root_node;
    SceneNode* m_scene_one_layer_node;

    Map* m_scene_one_map;
};
