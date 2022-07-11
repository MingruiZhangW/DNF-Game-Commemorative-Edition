#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "player.hpp"
#include "npc.hpp"
#include "sceneone.hpp"
#include "dialogscenenode.hpp"

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
    Player* getPlayer();
    void movePlayer(Player::PlayerMoveDir moveDir);

    // NPC
    NPC* getNPC();

    // Map
    glm::vec4 getCurrentSceneMapBoundary();

    // Process mouse move
    void processMouseMove(glm::vec2 mousePos);

    // Dialog
    void moveDialog(float dx);

private:
    enum class CurrentScene { SceneOne };

    void drawSceneOne();
    void reorderCurrentSceneLayerNode();
    void renderSceneGraphNodes(SceneNode* node, glm::mat4 modelMat);

    ShaderProgram* m_shader;
    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    CurrentScene m_current_scene;

    // Const per scene
    std::unique_ptr<Player> m_player;
    std::unique_ptr<NPC> m_npc;
    std::unique_ptr<DialogSceneNode> m_dialog_scene_node;

    std::unique_ptr<SceneOne> m_scene_one;
};
