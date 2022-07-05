#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "player.hpp"
#include "sceneone.h"

#include "GL/glew.h"

#include <memory>

class SceneManager
{
public:
    SceneManager(ShaderProgram* shader, GLfloat frameBufferWidth, GLfloat frameBufferHeight);
    ~SceneManager();

    // Scenes
    void constructScenes();
    void drawSceneOne();

    // Player
    void movePlayer(Player::PlayerMoveDir moveDir);

private:
    void renderSceneGraphNodes(SceneNode* node, glm::mat4 modelMat);

    ShaderProgram* m_shader;
    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    std::unique_ptr<Player> m_player;

    std::unique_ptr<SceneOne> m_scene_one;
};
