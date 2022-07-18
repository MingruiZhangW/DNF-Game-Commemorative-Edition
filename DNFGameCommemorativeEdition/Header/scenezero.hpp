#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "constant.hpp"

#include <memory>

class Background;
class Button;

class SceneZero
{
public:
    // TODO: add finite state machine.
    enum class SceneZeroStage { MainPage, ControlPage };

    SceneZero(ShaderProgram* shader, GLfloat frameBufferWidth, GLfloat frameBufferHeight);

    ~SceneZero();

    SceneNode* getRootSceneNode()
    {
        return m_scene_zero_root_node.get();
    }

    // NPC click, hover event handle
    // processHover return true when new button is hovered
    bool processHover(const glm::vec2& mousePos);
    Scene::SceneEvents processClick();

    // Initial display, should call first.
    void prepareInitialDisplay();

private:
    enum class CurrentButtonHoverInScene { None, Play, Exit, Control, Back };

    void construct();

    ShaderProgram* m_shader;

    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    // Scene node
    std::unique_ptr<SceneNode> m_scene_zero_root_node;

    // Components
    std::unique_ptr<Background> m_scene_zero_background;
    std::unique_ptr<Button> m_play_button;
    std::unique_ptr<Button> m_back_button;
    std::unique_ptr<Button> m_control_button;
    std::unique_ptr<Button> m_exit_button;
    std::unique_ptr<Button> m_dnf_logo;
    std::unique_ptr<Button> m_mouse_left;
    std::unique_ptr<Button> m_z_key;
    std::unique_ptr<Button> m_x_key;
    std::unique_ptr<Button> m_arrow_key;
    std::unique_ptr<Button> m_move_label;
    std::unique_ptr<Button> m_attack_label;
    std::unique_ptr<Button> m_interact_label;

    CurrentButtonHoverInScene m_current_button_hover;
    SceneZeroStage m_current_scene_stage;
};
