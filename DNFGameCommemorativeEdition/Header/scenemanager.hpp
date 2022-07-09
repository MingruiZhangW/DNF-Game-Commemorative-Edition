#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "player.hpp"
#include "sceneone.hpp"

#include "GL/glew.h"

#include <memory>

class SceneManager
{
public:
    SceneManager(ShaderProgram* shader, GLfloat frameBufferWidth, GLfloat frameBufferHeight);
    ~SceneManager();

    // Scenes
    void constructScenes();
    void drawCurrentScene();

    // Player
    void movePlayer(Player::PlayerMoveDir moveDir);
    float getPlayerDx();

    // Map
    glm::vec4 getCurrentSceneMapBoundary();

private:
    enum class CurrentScene { SceneOne };

    void drawSceneOne();
    void reorderCurrentSceneLayerNode();
    void renderSceneGraphNodes(SceneNode* node, glm::mat4 modelMat);

    ShaderProgram* m_shader;
    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    CurrentScene m_current_scene;

    std::unique_ptr<Player> m_player;

    std::unique_ptr<SceneOne> m_scene_one;
};
