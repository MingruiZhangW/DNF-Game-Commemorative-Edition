#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "player.hpp"

#include "GL/glew.h"

#include <memory>

class SceneOne
{
public:
    SceneOne(ShaderProgram* shader,
             GLfloat frameBufferWidth,
             GLfloat frameBufferHeight,
             Player* player);

    ~SceneOne();

    SceneNode* getRootSceneNode()
    {
        return m_scene_one_root_node.get();
    }

private:
    void construct();

    ShaderProgram* m_shader;
    Player* m_player;
    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    std::unique_ptr<SceneNode> m_scene_one_root_node;
};
