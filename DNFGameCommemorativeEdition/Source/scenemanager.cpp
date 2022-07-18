#include "scenemanager.hpp"

#include "game.hpp"
#include "glerrorcheck.hpp"
#include "monster.hpp"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/io.hpp>
#include <gtx/string_cast.hpp>

// Update mesh specific shader uniforms:
static void
updateShaderUniforms(const ShaderProgram* shader, const glm::mat4& nodeTrans)
{
    // No need to enable shader since within draw of Game
    {
        //-- Set ModelView matrix:
        auto M_uni = shader->getUniformLocation("M");

        glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(nodeTrans));
        CHECK_GL_ERRORS;
    }
}

SceneManager::SceneManager(ShaderProgram* shader,
                           GLfloat frameBufferWidth,
                           GLfloat frameBufferHeight,
                           GLFWwindow* window)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_player(std::make_unique<Player>(m_shader))
    , m_npc(std::make_unique<NPC>(m_shader))
    , m_dialog_scene_node(
          std::make_unique<DialogSceneNode>(m_shader, frameBufferWidth, frameBufferHeight))
    , m_star_particles_generator(std::make_unique<StarParticlesGenerator>(m_shader))
    , m_current_scene_state(CurrentSceneState::SceneZeroPrep)
    , m_window(window)
{
    // Init Sounds
    m_scene_zero_bg = Game::getSoundEngine()->addSoundSourceFromFile(SoundPath::sceneZeroBg.c_str());
    m_button_hover = Game::getSoundEngine()->addSoundSourceFromFile(SoundPath::buttonHover.c_str());
    m_button_click = Game::getSoundEngine()->addSoundSourceFromFile(SoundPath::buttonClick.c_str());
    m_scenes_bg = Game::getSoundEngine()->addSoundSourceFromFile(SoundPath::sceneOneTwoBg.c_str());
}

SceneManager::~SceneManager() {}

void
SceneManager::constructScenes()
{
    m_scene_zero = std::make_unique<SceneZero>(m_shader,
                                               m_frame_buffer_width,
                                               m_frame_buffer_height);

    m_scene_one = std::make_unique<SceneOne>(m_shader,
                                             m_frame_buffer_width,
                                             m_frame_buffer_height,
                                             m_player.get(),
                                             m_npc.get(),
                                             m_dialog_scene_node.get());

    m_scene_two = std::make_unique<SceneTwo>(m_shader,
                                             m_frame_buffer_width,
                                             m_frame_buffer_height,
                                             m_player.get(),
                                             m_dialog_scene_node.get(),
                                             m_star_particles_generator.get());
}

void
SceneManager::drawCurrentScene()
{
    switch (m_current_scene_state) {
    case CurrentSceneState::SceneZeroPrep:
        m_scene_zero->prepareInitialDisplay();

        if (!Game::getSoundEngine()->isCurrentlyPlaying(m_scene_zero_bg))
            m_scene_zero_bg_sound = Game::getSoundEngine()->play2D(m_scene_zero_bg,
                                                                   true,
                                                                   false,
                                                                   true);

        m_current_scene_state = CurrentSceneState::SceneZeroReady;
        break;
    case CurrentSceneState::SceneZeroReady:
        drawSceneZero();
        break;
    case CurrentSceneState::SceneOnePrep:
        m_scene_one->prepareInitialDisplay();
        m_scene_zero_bg_sound->stop();
        m_scene_zero_bg_sound->drop();

        // Scene zero bgm
        if (!Game::getSoundEngine()->isCurrentlyPlaying(m_scenes_bg))
            m_scenes_bg_sound = Game::getSoundEngine()->play2D(m_scenes_bg, true, false, true);

        m_current_scene_state = CurrentSceneState::SceneOneReady;
        break;
    case CurrentSceneState::SceneOneReady:
        drawSceneOne();
        break;
    case CurrentSceneState::SceneTwoPrep:
        m_scene_two->prepareInitialDisplay();

        m_current_scene_state = CurrentSceneState::SceneTwoReady;
        break;
    case CurrentSceneState::SceneTwoReady:
        drawSceneTwo();
        break;
    default:
        break;
    }
}

void
SceneManager::drawSceneOne()
{
    renderSceneGraphNodes(m_scene_one->getRootSceneNode(),
                          m_scene_one->getRootSceneNode()->getTransform());
}

void
SceneManager::drawSceneTwo()
{
    renderSceneGraphNodes(m_scene_two->getRootSceneNode(),
                          m_scene_two->getRootSceneNode()->getTransform());

    m_scene_two->checkToRemoveMonster();
    m_scene_two->updateMonsterFlockingMovements();
}

void
SceneManager::drawSceneZero()
{
    renderSceneGraphNodes(m_scene_zero->getRootSceneNode(),
                          m_scene_zero->getRootSceneNode()->getTransform());
}

void
SceneManager::renderSceneGraphNodes(SceneNode* node, glm::mat4 modelMat)
{
    if (node == nullptr)
        return;

    glm::mat4 trans {1.0f};
    if (node->m_node_type == NodeType::GeometryNode) {
        trans = modelMat * node->getTransform();
        updateShaderUniforms(m_shader, trans);
        m_player->updateShadowShaderModelMat(trans);
        m_npc->updateShadowShaderModelMat(trans);
        m_star_particles_generator->updateParticleShaderModelMat(trans);
        if (m_current_scene_state == CurrentSceneState::SceneTwoReady) {
            for (auto& i : m_scene_two->getMonsters()) {
                i->updateShadowShaderModelMat(trans);
            }
        }

        node->draw();
    }

    // All the children except the last
    for (auto i : node->m_children) {
        renderSceneGraphNodes(i, trans);
    }
}

void
SceneManager::movePlayer(Player::PlayerMoveDir moveDir)
{
    m_player->move(moveDir);

    std::pair<bool, std::pair<bool, bool>> collided;
    switch (m_current_scene_state) {
    case CurrentSceneState::SceneOneReady:
        // Check whether the door is hit
        collided = m_scene_one->sceneOneCollisionTest(m_player->getPlayerMovementAmount());
        if (collided.first) {
            m_player->cleanMovement();
            m_current_scene_state = CurrentSceneState::SceneTwoPrep;
            return;
        }
        m_player->reverseMove(collided.second);
        break;
    case CurrentSceneState::SceneTwoReady:
        m_player->reverseMove(
            m_scene_two->sceneTwoCollisionTest(m_player->getPlayerMovementAmount()));
        break;
    default:
        break;
    }

    reorderCurrentSceneLayerNode();
}

void
SceneManager::playerAttack()
{
    switch (m_current_scene_state) {
    case CurrentSceneState::SceneTwoReady: {
        for (auto& i : m_scene_two->sceneTwoAttackCollisionTest()) {
            if (i.second)
                i.first->setMonsterMode(Monster::MonsterMode::Killed);
        }
        break;
    }
    default:
        break;
    }
}

glm::vec4
SceneManager::getCurrentSceneMapBoundary()
{
    switch (m_current_scene_state) {
    case CurrentSceneState::SceneOneReady:
        return m_scene_one->getSceneOneMapBoundary();
    case CurrentSceneState::SceneTwoReady:
        return m_scene_two->getSceneTwoMapBoundary();
    default:
        break;
    }

    return glm::vec4(0.0f);
}

void
SceneManager::reorderCurrentSceneLayerNode()
{
    switch (m_current_scene_state) {
    case CurrentSceneState::SceneOneReady:
        return m_scene_one->reorderLayerNodeChild();
    case CurrentSceneState::SceneTwoReady:
        return m_scene_two->reorderLayerNodeChild();
    default:
        break;
    }
}

void
SceneManager::processMouseMove(const glm::vec2& mousePos)
{
    switch (m_current_scene_state) {
    case CurrentSceneState::SceneZeroReady:
        if (m_scene_zero->processHover(mousePos)) {
            // Button hover sound
            if (Game::getSoundEngine()->isCurrentlyPlaying(m_button_hover)) {
                m_button_hover_sound->stop();
                m_button_hover_sound = Game::getSoundEngine()->play2D(m_button_hover,
                                                                      false,
                                                                      false,
                                                                      true);
            } else {
                m_button_hover_sound = Game::getSoundEngine()->play2D(m_button_hover,
                                                                      false,
                                                                      false,
                                                                      true);
            }
        }
        break;
    case CurrentSceneState::SceneOneReady:
        m_scene_one->processHover(mousePos);
        break;
    case CurrentSceneState::SceneTwoReady:
        if (m_scene_two->processHover(mousePos)) {
            // Button hover sound
            if (Game::getSoundEngine()->isCurrentlyPlaying(m_button_hover)) {
                m_button_hover_sound->stop();
                m_button_hover_sound = Game::getSoundEngine()->play2D(m_button_hover,
                                                                      false,
                                                                      false,
                                                                      true);
            } else {
                m_button_hover_sound = Game::getSoundEngine()->play2D(m_button_hover,
                                                                      false,
                                                                      false,
                                                                      true);
            }
        }
        break;
    default:
        break;
    }
}

void
SceneManager::processLeftMouseClick()
{
    switch (m_current_scene_state) {
    case CurrentSceneState::SceneZeroReady:
        if (m_scene_zero->processClick() == Scene::SceneEvents::QuitGame) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        } else if (m_scene_zero->processClick() == Scene::SceneEvents::SceneTransit) {
            m_current_scene_state = CurrentSceneState::SceneOnePrep;
        }
        break;
    case CurrentSceneState::SceneOneReady:
        if (m_scene_one->processClick()) {
            // Dialog click sound
            if (Game::getSoundEngine()->isCurrentlyPlaying(m_button_click)) {
                m_button_click_sound->stop();
                m_button_click_sound = Game::getSoundEngine()->play2D(m_button_click,
                                                                      false,
                                                                      false,
                                                                      true);
            } else {
                m_button_click_sound = Game::getSoundEngine()->play2D(m_button_click,
                                                                      false,
                                                                      false,
                                                                      true);
            }
        }
        break;
    case CurrentSceneState::SceneTwoReady: {
        auto currentEvent = m_scene_two->processClick();
        if (currentEvent == Scene::SceneEvents::DialogClick) {
            // Dialog click sound
            if (Game::getSoundEngine()->isCurrentlyPlaying(m_button_click)) {
                m_button_click_sound->stop();
                m_button_click_sound = Game::getSoundEngine()->play2D(m_button_click,
                                                                      false,
                                                                      false,
                                                                      true);
            } else {
                m_button_click_sound = Game::getSoundEngine()->play2D(m_button_click,
                                                                      false,
                                                                      false,
                                                                      true);
            }
        } else if (currentEvent == Scene::SceneEvents::QuitGame) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        }
        break;
    }
    default:
        break;
    }
}

void
SceneManager::moveDialog(float dx)
{
    switch (m_current_scene_state) {
    case CurrentSceneState::SceneOneReady:
        m_scene_one->moveDialog(dx);
        break;
    case CurrentSceneState::SceneTwoReady:
        m_scene_two->moveDialog(dx);
        break;
    default:
        break;
    }
}

Player*
SceneManager::getPlayer()
{
    return m_player.get();
}

NPC*
SceneManager::getNPC()
{
    return m_npc.get();
}

StarParticlesGenerator*
SceneManager::getStarParticlesGenerator()
{
    return m_star_particles_generator.get();
}

std::vector<Monster*>&
SceneManager::getMonsters()
{
    return m_scene_two->getMonsters();
}
