#include "scenemanager.hpp"

#include "glerrorcheck.hpp"

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
                           GLfloat frameBufferHeight)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_player(std::make_unique<Player>(m_shader))
    , m_npc(std::make_unique<NPC>(m_shader))
    , m_dialog_scene_node(
          std::make_unique<DialogSceneNode>(m_shader, frameBufferWidth, frameBufferHeight))
    , m_current_scene(CurrentScene::SceneOne)
{}

SceneManager::~SceneManager() {}

void
SceneManager::constructScenes()
{
    m_scene_one = std::make_unique<SceneOne>(m_shader,
                                             m_frame_buffer_width,
                                             m_frame_buffer_height,
                                             m_player.get(),
                                             m_npc.get(),
                                             m_dialog_scene_node.get());
}

void
SceneManager::drawCurrentScene()
{
    switch (m_current_scene) {
    case CurrentScene::SceneOne:
        drawSceneOne();
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
    m_player->reverseMove(m_scene_one->sceneOneCollisionTest(m_player->getPlayerMovementAmount()));

    reorderCurrentSceneLayerNode();
}

glm::vec4
SceneManager::getCurrentSceneMapBoundary()
{
    switch (m_current_scene) {
    case SceneManager::CurrentScene::SceneOne:
        return m_scene_one->getSceneOneMapBoundary();
    default:
        break;
    }

    return glm::vec4(0.0f);
}

void
SceneManager::reorderCurrentSceneLayerNode()
{
    switch (m_current_scene) {
    case SceneManager::CurrentScene::SceneOne:
        return m_scene_one->reorderLayerNodeChild();
    default:
        break;
    }
}

void
SceneManager::processMouseMove(const glm::vec2& mousePos)
{
    switch (m_current_scene) {
    case SceneManager::CurrentScene::SceneOne:
        m_scene_one->processHover(mousePos);
        break;
    default:
        break;
    }
}

void
SceneManager::processLeftMouseClick()
{
    switch (m_current_scene) {
    case SceneManager::CurrentScene::SceneOne:
        m_scene_one->processClick();
        break;
    default:
        break;
    }
}

void
SceneManager::moveDialog(float dx)
{
    switch (m_current_scene) {
    case SceneManager::CurrentScene::SceneOne:
        m_scene_one->moveDialog(dx);
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
