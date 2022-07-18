#include "game.hpp"
#include "glerrorcheck.hpp"
#include "constant.hpp"
#include "monster.hpp"

#include <iostream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/io.hpp>
#include <gtx/string_cast.hpp>

static irrklang::ISoundEngine* m_game_sound_engine = irrklang::createIrrKlangDevice();

// Keyboard event control
static bool m_enable_keyboard_event = true;

void
Game::enableKeyBoardEvent(bool enable)
{
    m_enable_keyboard_event = enable;
}

bool
Game::getEnableKeyBoardEvent()
{
    return m_enable_keyboard_event;
}

irrklang::ISoundEngine*
Game::getSoundEngine()
{
    return m_game_sound_engine;
}

// Constructor
Game::Game()
    : m_camera_pos(glm::vec3(0.0f, 0.0f, 0.0f))
    , m_camera_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_play_doing_basic_attack(false)
    , m_play_doing_skill_attack(false)
{}

// Destructor
Game::~Game()
{
    m_game_sound_engine->drop();
}

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
    m_view = glm::lookAt(m_camera_pos, m_camera_pos + m_camera_front, glm::vec3(0.0f, 1.0f, 0.0f));

    m_proj = glm::ortho(0.0f,
                        static_cast<float>(m_framebufferWidth),
                        0.0f,
                        static_cast<float>(m_framebufferHeight),
                        -1.0f,
                        1.0f);

    m_scene_manager = std::make_unique<SceneManager>(&m_shader,
                                                     static_cast<float>(m_framebufferWidth),
                                                     static_cast<float>(m_framebufferHeight),
                                                     m_window);

    m_scene_manager->constructScenes();
}

/*
 * Called once per frame, before guiLogic().
 */
void
Game::appLogic()
{
    handleInputKeys();
    if (m_scene_manager->getPlayer()->getPlayerMode() == Player::PlayerMode::BasicAttack
        || m_scene_manager->getPlayer()->getPlayerMode() == Player::PlayerMode::Skill) {
        m_scene_manager->playerAttack();
    }
    if (m_scene_manager->getCurrentSceneState() == SceneManager::CurrentSceneState::SceneTwoPrep
        || m_scene_manager->getCurrentSceneState()
               == SceneManager::CurrentSceneState::SceneZeroPrep) {
        m_camera_pos = glm::vec3(0.0f);
    }
    m_view = glm::lookAt(m_camera_pos, m_camera_pos + m_camera_front, glm::vec3(0.0f, 1.0f, 0.0f));
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
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader.enable();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    glUniformMatrix4fv(P_uni, 1, GL_FALSE, value_ptr(m_proj));
    glUniformMatrix4fv(V_uni, 1, GL_FALSE, value_ptr(m_view));
    m_scene_manager->getPlayer()->updateShadowShaderPVMat(m_proj, m_view);
    m_scene_manager->getNPC()->updateShadowShaderPVMat(m_proj, m_view);
    m_scene_manager->getStarParticlesGenerator()->updateParticleShaderPVMat(m_proj, m_view);
    for (auto& i : m_scene_manager->getMonsters()) {
        i->updateShadowShaderPVMat(m_proj, m_view);
    }

    // Main drawing
    m_scene_manager->drawCurrentScene();

    m_shader.disable();

    CHECK_GL_ERRORS;
}

/*
 * Transform mouse pos to world space
 */
glm::vec2
Game::convertMousePos(double xPos, double yPos)
{
    // To NDC and perspective divide is ignored
    glm::vec4 ndcMouse = glm::vec4(((2.0f * xPos) / m_framebufferWidth - 1.0f),
                                   (1.0f - (2.0f * yPos) / m_framebufferHeight),
                                   -1.0f,
                                   1.0f);

    // Inverse proj, view
    auto invPV = glm::inverse(m_view)
                 * glm::vec4(glm::vec2(glm::inverse(m_proj) * ndcMouse), -1.0f, 0.0f);

    return glm::vec2(invPV.x + m_camera_pos.x, invPV.y);
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

    m_scene_manager->processMouseMove(convertMousePos(xPos, yPos));

    return eventHandled;
}

/*
 * Event handler.  Handles mouse button events.
 */
bool
Game::mouseButtonInputEvent(int button, int actions, int mods)
{
    bool eventHandled(false);

    if (actions == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            m_scene_manager->processLeftMouseClick();
            eventHandled = true;
        }
    }

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

    // Don't accept unknown keys
    if (!m_enable_keyboard_event || key == GLFW_KEY_UNKNOWN)
        return eventHandled;

    // Store key sequences and deal with them in handleInputKeys()
    // Not for attacks
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_UP:
            if (m_player_walk_up_down_key_sequence.size() <= 2)
                m_player_walk_up_down_key_sequence.emplace_back(GLFWArrowKeyRemap::upKey);
            break;
        case GLFW_KEY_DOWN:
            if (m_player_walk_up_down_key_sequence.size() <= 2)
                m_player_walk_up_down_key_sequence.emplace_back(GLFWArrowKeyRemap::downKey);
            break;
        case GLFW_KEY_LEFT:
            if (m_player_walk_left_right_key_sequence.size() <= 2)
                m_player_walk_left_right_key_sequence.emplace_back(GLFWArrowKeyRemap::leftKey);
            break;
        case GLFW_KEY_RIGHT:
            if (m_player_walk_left_right_key_sequence.size() <= 2)
                m_player_walk_left_right_key_sequence.emplace_back(GLFWArrowKeyRemap::rightKey);
            break;
        case GLFW_KEY_X:
            m_play_doing_basic_attack = true;
            break;
        case GLFW_KEY_Z:
            m_play_doing_skill_attack = true;
            break;
        default:
            break;
        }

        eventHandled = true;
    } else if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_UP:
            for (auto it = m_player_walk_up_down_key_sequence.begin();
                 it != m_player_walk_up_down_key_sequence.end();) {
                if (*it == GLFWArrowKeyRemap::upKey) {
                    m_player_walk_up_down_key_sequence.erase(it);
                    break;
                } else {
                    ++it;
                }
            }
            break;
        case GLFW_KEY_DOWN:
            for (auto it = m_player_walk_up_down_key_sequence.begin();
                 it != m_player_walk_up_down_key_sequence.end();) {
                if (*it == GLFWArrowKeyRemap::downKey) {
                    m_player_walk_up_down_key_sequence.erase(it);
                    break;
                } else {
                    ++it;
                }
            }
            break;
        case GLFW_KEY_LEFT:
            for (auto it = m_player_walk_left_right_key_sequence.begin();
                 it != m_player_walk_left_right_key_sequence.end();) {
                if (*it == GLFWArrowKeyRemap::leftKey) {
                    m_player_walk_left_right_key_sequence.erase(it);
                    break;
                } else {
                    ++it;
                }
            }
            break;
        case GLFW_KEY_RIGHT:
            for (auto it = m_player_walk_left_right_key_sequence.begin();
                 it != m_player_walk_left_right_key_sequence.end();) {
                if (*it == GLFWArrowKeyRemap::rightKey) {
                    m_player_walk_left_right_key_sequence.erase(it);
                    break;
                } else {
                    ++it;
                }
            }
            break;
        default:
            break;
        }

        eventHandled = true;
    }

    return eventHandled;
}

void
Game::handleInputKeys()
{
    // Clear keys
    if (!m_enable_keyboard_event) {
        m_player_walk_up_down_key_sequence.clear();
        m_player_walk_left_right_key_sequence.clear();
        m_player_run_key_up_down_sequence.clear();
        m_player_run_key_left_right_sequence.clear();
    }

    // Handle attack
    if (m_scene_manager->getPlayer()->lockForMovement())
        return;

    if (m_play_doing_basic_attack) {
        m_scene_manager->getPlayer()->setPlayerMode(Player::PlayerMode::BasicAttack);
        m_play_doing_basic_attack = false;

        return;
    } else if (m_play_doing_skill_attack) {
        m_scene_manager->getPlayer()->setPlayerMode(Player::PlayerMode::Skill);
        m_play_doing_skill_attack = false;

        return;
    }

    // Use the int property of enum
    // calculate the dir for the player
    int dominant_updown {0};
    int dominant_leftright {0};

    for (auto i : m_player_walk_up_down_key_sequence) {
        if (i == GLFWArrowKeyRemap::upKey || i == GLFWArrowKeyRemap::downKey)
            dominant_updown = i;
    }

    for (auto i : m_player_walk_left_right_key_sequence) {
        if (i == GLFWArrowKeyRemap::leftKey || i == GLFWArrowKeyRemap::rightKey)
            dominant_leftright = i;
    }

    if (dominant_updown + dominant_leftright != 0) {
        m_scene_manager->getPlayer()->setPlayerMode(Player::PlayerMode::Walk);

        m_scene_manager->movePlayer(
            static_cast<Player::PlayerMoveDir>(dominant_updown + dominant_leftright));

        processCameraMove();

        return;
    }

    m_scene_manager->getPlayer()->setPlayerMode(Player::PlayerMode::Stand);
}

void
Game::processCameraMove()
{
    // Start to move the camera when the player is in the middle of the window.
    // Do not move when the player reaches the edges.
    auto playDx = m_scene_manager->getPlayer()->getPlayerDx();
    auto mapWidthBound = m_scene_manager->getCurrentSceneMapBoundary().w
                         - m_framebufferWidth / 2.0f;
    auto halfWidthLine = m_framebufferWidth / 2.0f;
    auto cameraXNow = m_camera_pos.x;

    if (playDx > halfWidthLine && playDx < mapWidthBound) {
        m_camera_pos.x = playDx - halfWidthLine;
        m_scene_manager->moveDialog(m_camera_pos.x - cameraXNow);
    }
}
