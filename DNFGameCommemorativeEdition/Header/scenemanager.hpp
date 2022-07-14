#pragma once

#include "scenenode.hpp"
#include "scenezero.hpp"
#include "gamewindow.hpp"
#include "shaderprogram.hpp"
#include "player.hpp"
#include "npc.hpp"
#include "sceneone.hpp"
#include "dialogscenenode.hpp"

#include "GL/glew.h"

#include <irrKlang.h>

#include <memory>

class SceneManager
{
public:
    SceneManager(ShaderProgram* shader,
                 GLfloat frameBufferWidth,
                 GLfloat frameBufferHeight,
                 GLFWwindow* window);
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

    // Process mouse move and left mouse click.
    void processMouseMove(const glm::vec2& mousePos);
    void processLeftMouseClick();

    // Dialog
    void moveDialog(float dx);

private:
    enum class CurrentSceneState { SceneZeroPrep, SceneZeroReady, SceneOnePrep, SceneOneReady };

    void drawSceneOne();
    void drawSceneZero();
    void reorderCurrentSceneLayerNode();
    void renderSceneGraphNodes(SceneNode* node, glm::mat4 modelMat);

    ShaderProgram* m_shader;
    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    CurrentSceneState m_current_scene_state;

    // Const per scene
    std::unique_ptr<Player> m_player;
    std::unique_ptr<NPC> m_npc;
    std::unique_ptr<DialogSceneNode> m_dialog_scene_node;

    // Scenes
    std::unique_ptr<SceneOne> m_scene_one;
    std::unique_ptr<SceneZero> m_scene_zero;

    // Sound and sound source
    irrklang::ISound* m_scene_zero_bg_sound;
    irrklang::ISound* m_button_hover_sound;
    irrklang::ISound* m_button_click_sound;

    irrklang::ISoundSource* m_scene_zero_bg;
    irrklang::ISoundSource* m_button_hover;
    irrklang::ISoundSource* m_button_click;

    GLFWwindow* m_window;
};
