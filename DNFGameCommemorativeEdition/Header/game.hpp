#pragma once

#include "gamewindow.hpp"
#include "shaderprogram.hpp"
#include "background.hpp"
#include "scenemanager.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>

#include <memory>

class Game : public GameWindow
{
public:
    Game();
    virtual ~Game();

protected:
    virtual void init() override;
    virtual void appLogic() override;
    virtual void guiLogic() override;
    virtual void draw() override;
    virtual void cleanup() override;

    virtual bool cursorEnterWindowEvent(int entered) override;
    virtual bool mouseMoveEvent(double xPos, double yPos) override;
    virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
    virtual bool windowResizeEvent(int width, int height) override;
    virtual bool keyInputEvent(int key, int action, int mods) override;

    // Thin layer for input key handling
    void handleInputKeys();

    // Handle player-follow camera
    void processCameraMove();

    std::unique_ptr<SceneManager> m_scene_manager;

    ShaderProgram m_shader;
    GLint P_uni; // Uniform location for Projection matrix.
    GLint V_uni; // Uniform location for View matrix.
    GLint M_uni; // Uniform location for Model matrix.

    // Matrices controlling the camera and projection.
    glm::mat4 m_proj;
    glm::mat4 m_view;
    glm::vec3 m_camera_pos;
    glm::vec3 m_camera_front;

    // Player movement
    std::vector<int> m_player_walk_up_down_key_sequence;
    std::vector<int> m_player_walk_left_right_key_sequence;
    std::vector<int> m_player_run_key_up_down_sequence;
    std::vector<int> m_player_run_key_left_right_sequence;
};
