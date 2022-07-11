#include "dialogscenenode.hpp"
#include "constant.hpp"

DialogSceneNode::DialogSceneNode(ShaderProgram* shader,
                                 GLfloat frameBufferWidth,
                                 GLfloat frameBufferHeight)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_dialog(new Dialog(shader, frameBufferWidth, frameBufferHeight))
    , m_dialog_image(new DialogImage(shader))
    , m_dialog_root_node(std::make_unique<SceneNode>(StringContant::dialogRootNodeName))
{
    construct();
}

DialogSceneNode::~DialogSceneNode() {}

void
DialogSceneNode::construct()
{
    m_dialog_image->setDialogOrigin(m_dialog->getOriginTrans());

    m_dialog_root_node->addChild(m_dialog);
    m_dialog_root_node->addChild(m_dialog_image);
}

void
DialogSceneNode::moveDialog(const glm::vec3& amount)
{
    m_dialog->translate(amount);
    m_dialog_image->translate(amount);
}