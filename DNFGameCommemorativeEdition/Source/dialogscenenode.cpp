#include "dialogscenenode.hpp"
#include "constant.hpp"

#include <gtx/transform.hpp>

DialogSceneNode::DialogSceneNode(ShaderProgram* shader,
                                 GLfloat frameBufferWidth,
                                 GLfloat frameBufferHeight)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_dialog(new Dialog(shader, frameBufferWidth, frameBufferHeight))
    , m_dialog_image(new DialogImage(shader))
    , m_is_dialog_shown(false)
    , m_dialog_text_manager(std::make_unique<DialogTextManager>(shader))
    , m_dialog_root_node(std::make_unique<SceneNode>(StringContant::dialogRootNodeName))
{
    construct();
}

DialogSceneNode::~DialogSceneNode() {}

void
DialogSceneNode::construct()
{
    m_dialog_image->setDialogOrigin(m_dialog->getOriginTrans());
    m_dialog_text_manager->setDialogTextInitialTrans(m_dialog->getOriginTrans());
    m_dialog_text_manager->setDialogNameShift(
        glm::vec2(DialogSize::dialogNameXOffset,
                  m_dialog->getTextureGeo().y + DialogSize::dialogNameYOffset));
    m_dialog_text_manager->setDialogTextShift(
        glm::vec2(DialogSize::dialogTextXOffset,
                  m_dialog->getTextureGeo().y + DialogSize::dialogTextYOffset));

    m_dialog_root_node->addChild(m_dialog);
    m_dialog_root_node->addChild(m_dialog_image);
}

void
DialogSceneNode::moveDialog(const glm::vec3& amount)
{
    m_dialog->translate(amount);
    m_dialog_image->translate(amount);

    if (m_is_dialog_shown) {
        for (auto& i : m_current_dialog_text_chars) {
            i->translate(amount);
        }

        for (auto& i : m_current_dialog_speaker_chars) {
            i->translate(amount);
        }
    }

    auto currentTextTrans = m_dialog_text_manager->getDialogTextInitialTrans();

    m_dialog_text_manager->setDialogTextInitialTrans(
        glm::vec2(currentTextTrans.x + amount.x, currentTextTrans.y + amount.y));
}

void
DialogSceneNode::setCurrentDialogText(const std::string& dialogText)
{
    m_current_dialog_string = dialogText;

    for (auto& i : m_current_dialog_text_chars) {
        m_dialog_root_node->removeChild(i);
        delete i;
    }

    m_current_dialog_text_chars.clear();
    m_current_dialog_text_chars = m_dialog_text_manager->getDialogTextCharNodeListFromString(
        m_current_dialog_string);

    for (auto i : m_current_dialog_text_chars) {
        m_dialog_root_node->addChild(i);
    }
}

void
DialogSceneNode::setCurrentDialogSpeaker(const std::wstring& speakerName)
{
    m_current_dialog_speaker_string = speakerName;

    for (auto i : m_current_dialog_speaker_chars) {
        m_dialog_root_node->removeChild(i);
        delete i;
    }

    m_current_dialog_speaker_chars.clear();
    m_current_dialog_speaker_chars = m_dialog_text_manager->getDialogNameCharNodeListFromString(
        m_current_dialog_speaker_string);

    for (auto i : m_current_dialog_speaker_chars) {
        m_dialog_root_node->addChild(i);
    }
}