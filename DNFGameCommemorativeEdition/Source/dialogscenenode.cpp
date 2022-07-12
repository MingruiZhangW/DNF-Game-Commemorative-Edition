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

    m_current_dialog_speaker_chars = m_dialog_text_manager->getDialogNameCharNodeListFromString(
        L"ÖËçü");
    for (auto i : m_current_dialog_speaker_chars) {
        m_dialog_root_node->addChild(i);
    }

    m_current_dialog_text_chars = m_dialog_text_manager->getDialogTextCharNodeListFromString(
        "Hi, my name is dsjksdjksdjskdjk. sdkakldjaklsdjaklsfnsad "
        "sdaddafffffffffffffffffffffffffffffffffffffsrgtrgfsefeafdaefa");
    for (auto i : m_current_dialog_text_chars) {
        m_dialog_root_node->addChild(i);
    }
}

void
DialogSceneNode::moveDialog(const glm::vec3& amount)
{
    m_dialog->translate(amount);
    m_dialog_image->translate(amount);

    for (auto& i : m_current_dialog_text_chars) {
        i->translate(amount);
    }

    for (auto& i : m_current_dialog_speaker_chars) {
        i->translate(amount);
    }
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
}