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
{}

SceneManager::~SceneManager() {}

void
SceneManager::constructScenes()
{
    m_scene_one = std::make_unique<SceneOne>(m_shader,
                                             m_frame_buffer_width,
                                             m_frame_buffer_height,
                                             m_player.get());
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
}