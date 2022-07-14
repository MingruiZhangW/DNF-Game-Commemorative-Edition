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
    enum class CurrentButtonHoverInScene { None, Play, Exit };

    void construct();

    ShaderProgram* m_shader;

    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    std::unique_ptr<SceneNode> m_scene_zero_root_node;

    Background* m_scene_zero_background;
    Button* m_play_button;
    Button* m_exit_button;
    Button* m_dnf_logo;

    CurrentButtonHoverInScene m_current_button_hover;
};
