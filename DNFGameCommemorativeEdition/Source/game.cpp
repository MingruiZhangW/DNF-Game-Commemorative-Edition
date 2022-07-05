#include "game.hpp"
#include "glerrorcheck.hpp"
#include "constant.hpp"

#include <iostream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/io.hpp>
#include <gtx/string_cast.hpp>

// Constructor
Game::Game() {}

// Destructor
Game::~Game() {}

/*
 * Called once, at program start.
 */
void
Game::init()
{
    // Build the shader
    m_shader.generateProgramObject();
    m_shader.attachVertexShader(getResourceFilePath("Shaders/shader.vert").c_str());
    m_shader.attachFragmentShader(getResourceFilePath("Shaders/shader.frag").c_str());
    m_shader.link();

    // Set up the uniforms
    P_uni = m_shader.getUniformLocation("P");
    V_uni = m_shader.getUniformLocation("V");
    M_uni = m_shader.getUniformLocation("M");

    // Set up initial view and projection matrices (need to do this here,
    // since it depends on the GLFW window being set up correctly).
    // m_view = glm::lookAt(glm::vec3(0.0f, 16.0f, 16.0f),
    //                     glm::vec3(0.0f, 0.0f, 0.0f),
    //                     glm::vec3(0.0f, 1.0f, 0.0f));

    m_view = glm::mat4(1.0f);
    m_proj = glm::ortho(0.0f,
                        static_cast<float>(m_framebufferWidth),
                        0.0f,
                        static_cast<float>(m_framebufferHeight),
                        -1.0f,
                        1.0f);

    m_scene_manager = std::make_unique<SceneManager>(&m_shader,
                                                     static_cast<float>(m_framebufferWidth),
                                                     static_cast<float>(m_framebufferHeight));

    m_scene_manager->constructScenes();
}

/*
 * Called once per frame, before guiLogic().
 */
void
Game::appLogic()
{
    // Place per frame, application logic here ...
}

/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void
Game::guiLogic()
{}

/*
 * Called once per frame, after guiLogic().
 */
void
Game::draw()
{
    //// Create a global transformation for the model (centre it).
    // glm::mat4 W {1.0f};
    //// W = glm::translate(W, glm::vec3(-float(16) / 2.0f, 0, -float(16) / 2.0f));
    // W = glm::scale(W, glm::vec3(m_framebufferWidth, m_framebufferHeight, 0.0f));
    //// W = glm::rotate(W, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader.enable();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glUniformMatrix4fv(P_uni, 1, GL_FALSE, value_ptr(m_proj));
    glUniformMatrix4fv(V_uni, 1, GL_FALSE, value_ptr(m_view));

    // Main drawing
    m_scene_manager->drawSceneOne();

    m_shader.disable();

    CHECK_GL_ERRORS;
}

/*
 * Called once, after program is signaled to terminate.
 */
void
Game::cleanup()
{}

/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool
Game::cursorEnterWindowEvent(int entered)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool
Game::mouseMoveEvent(double xPos, double yPos)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles mouse button events.
 */
bool
Game::mouseButtonInputEvent(int button, int actions, int mods)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool
Game::mouseScrollEvent(double xOffSet, double yOffSet)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles window resize events.
 */
bool
Game::windowResizeEvent(int width, int height)
{
    bool eventHandled(false);

    return eventHandled;
}

/*
 * Event handler.  Handles key input events.
 */
bool
Game::keyInputEvent(int key, int action, int mods)
{
    bool eventHandled(false);

    // Use the int property of enum
    // calculate the dir for the player
    int dominant_updown {0};
    int dominant_leftright {0};
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_UP:
            if (m_player_walk_key_sequence.size() > 3)
                return eventHandled;
            m_player_walk_key_sequence.emplace_back(GLFWArrowKeyRemap::upKey);

            for (auto i : m_player_walk_key_sequence) {
                if (i == GLFWArrowKeyRemap::upKey || i == GLFWArrowKeyRemap::downKey)
                    dominant_updown = i;
                if (i == GLFWArrowKeyRemap::leftKey || i == GLFWArrowKeyRemap::rightKey)
                    dominant_leftright = i;
            }

            m_scene_manager->movePlayer(
                static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

            eventHandled = true;
            break;
        case GLFW_KEY_DOWN:
            if (m_player_walk_key_sequence.size() > 3)
                return eventHandled;
            ;
            m_player_walk_key_sequence.emplace_back(GLFWArrowKeyRemap::downKey);

            for (auto i : m_player_walk_key_sequence) {
                if (i == GLFWArrowKeyRemap::upKey || i == GLFWArrowKeyRemap::downKey)
                    dominant_updown = i;
                if (i == GLFWArrowKeyRemap::leftKey || i == GLFWArrowKeyRemap::rightKey)
                    dominant_leftright = i;
            }

            m_scene_manager->movePlayer(
                static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

            eventHandled = true;
            break;
        case GLFW_KEY_LEFT:
            if (m_player_walk_key_sequence.size() > 3)
                return eventHandled;
            ;
            m_player_walk_key_sequence.emplace_back(GLFWArrowKeyRemap::leftKey);

            for (auto i : m_player_walk_key_sequence) {
                if (i == GLFWArrowKeyRemap::upKey || i == GLFWArrowKeyRemap::downKey)
                    dominant_updown = i;
                if (i == GLFWArrowKeyRemap::leftKey || i == GLFWArrowKeyRemap::rightKey)
                    dominant_leftright = i;
            }

            m_scene_manager->movePlayer(
                static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

            eventHandled = true;
            break;
        case GLFW_KEY_RIGHT:
            if (m_player_walk_key_sequence.size() > 3)
                return eventHandled;
            ;
            m_player_walk_key_sequence.emplace_back(GLFWArrowKeyRemap::rightKey);

            for (auto i : m_player_walk_key_sequence) {
                if (i == GLFWArrowKeyRemap::upKey || i == GLFWArrowKeyRemap::downKey)
                    dominant_updown = i;
                if (i == GLFWArrowKeyRemap::leftKey || i == GLFWArrowKeyRemap::rightKey)
                    dominant_leftright = i;
            }

            m_scene_manager->movePlayer(
                static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

            eventHandled = true;
            break;
        default:
            break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_UP:
            for (auto it = m_player_walk_key_sequence.begin();
                 it != m_player_walk_key_sequence.end();) {
                if (*it == GLFWArrowKeyRemap::downKey)
                    dominant_updown = *it;
                if (*it == GLFWArrowKeyRemap::leftKey || *it == GLFWArrowKeyRemap::rightKey)
                    dominant_leftright = *it;

                if (*it = GLFWArrowKeyRemap::upKey) {
                    m_player_walk_key_sequence.erase(it);
                } else {
                    ++it;
                }
            }

            m_scene_manager->movePlayer(
                static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

            eventHandled = true;
            break;
        case GLFW_KEY_DOWN:
            for (auto it = m_player_walk_key_sequence.begin();
                 it != m_player_walk_key_sequence.end();) {
                if (*it == GLFWArrowKeyRemap::upKey)
                    dominant_updown = *it;
                if (*it == GLFWArrowKeyRemap::leftKey || *it == GLFWArrowKeyRemap::rightKey)
                    dominant_leftright = *it;

                if (*it = GLFWArrowKeyRemap::downKey) {
                    m_player_walk_key_sequence.erase(it);
                } else {
                    ++it;
                }
            }

            m_scene_manager->movePlayer(
                static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

            eventHandled = true;
            break;
        case GLFW_KEY_LEFT:
            for (auto it = m_player_walk_key_sequence.begin();
                 it != m_player_walk_key_sequence.end();) {
                if (*it == GLFWArrowKeyRemap::upKey || GLFWArrowKeyRemap::downKey)
                    dominant_updown = *it;
                if (*it == GLFWArrowKeyRemap::rightKey)
                    dominant_leftright = *it;

                if (*it = GLFWArrowKeyRemap::leftKey) {
                    m_player_walk_key_sequence.erase(it);
                } else {
                    ++it;
                }
            }

            m_scene_manager->movePlayer(
                static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

            eventHandled = true;
            break;
        case GLFW_KEY_RIGHT:
            for (auto it = m_player_walk_key_sequence.begin();
                 it != m_player_walk_key_sequence.end();) {
                if (*it == GLFWArrowKeyRemap::upKey || GLFWArrowKeyRemap::downKey)
                    dominant_updown = *it;
                if (*it == GLFWArrowKeyRemap::leftKey)
                    dominant_leftright = *it;

                if (*it = GLFWArrowKeyRemap::rightKey) {
                    m_player_walk_key_sequence.erase(it);
                } else {
                    ++it;
                }
            }

            m_scene_manager->movePlayer(
                static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

            eventHandled = true;
            break;
        default:
            break;
        }
    }

    return eventHandled;
}
