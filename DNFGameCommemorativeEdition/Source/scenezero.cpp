#include "scenezero.hpp"
#include "background.hpp"
#include "button.hpp"
#include "game.hpp"

// Offset
const static float m_dnf_logo_offset {30.0f};
const static float m_buttons_offset {120.0f};

SceneZero::SceneZero(ShaderProgram* shader, GLfloat frameBufferWidth, GLfloat frameBufferHeight)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_scene_zero_root_node(std::make_unique<SceneNode>(StringContant::sceneZeroRootNodeName))
    , m_current_button_hover(CurrentButtonHoverInScene::None)
    , m_current_scene_stage(SceneZeroStage::MainPage)
{
    construct();
}

SceneZero::~SceneZero()
{
    // Remove potential existing node
    m_scene_zero_root_node->cleanChild();
}

void
SceneZero::construct()
{
    // Create components
    m_scene_zero_background
        = std::make_unique<Background>(StringContant::sceneZeroBackgroundName,
                                       m_shader,
                                       m_frame_buffer_width,
                                       m_frame_buffer_height,
                                       Background::BackgroundTextureType::SceneZero);

    m_play_button = std::make_unique<Button>(StringContant::buttonName + "play",
                                             m_shader,
                                             Button::ButtonTextureType::PlayButton);

    m_exit_button = std::make_unique<Button>(StringContant::buttonName + "exit",
                                             m_shader,
                                             Button::ButtonTextureType::ExitButton);

    m_control_button = std::make_unique<Button>(StringContant::buttonName + "control",
                                                m_shader,
                                                Button::ButtonTextureType::ControlButton);

    m_back_button = std::make_unique<Button>(StringContant::buttonName + "back",
                                             m_shader,
                                             Button::ButtonTextureType::BackButton);

    m_mouse_left = std::make_unique<Button>(StringContant::buttonName + "mouseLeft",
                                            m_shader,
                                            Button::ButtonTextureType::MouseLeft);

    m_z_key = std::make_unique<Button>(StringContant::buttonName + "zKey",
                                       m_shader,
                                       Button::ButtonTextureType::ZKey);

    m_x_key = std::make_unique<Button>(StringContant::buttonName + "xKey",
                                       m_shader,
                                       Button::ButtonTextureType::XKey);

    m_arrow_key = std::make_unique<Button>(StringContant::buttonName + "arrowKey",
                                           m_shader,
                                           Button::ButtonTextureType::ArrowKey);

    m_move_label = std::make_unique<Button>(StringContant::buttonName + "moveLabel",
                                            m_shader,
                                            Button::ButtonTextureType::MoveLabel);

    m_attack_label = std::make_unique<Button>(StringContant::buttonName + "attackLabel",
                                              m_shader,
                                              Button::ButtonTextureType::AttackLabel);

    m_interact_label = std::make_unique<Button>(StringContant::buttonName + "interactLabel",
                                                m_shader,
                                                Button::ButtonTextureType::InteractLabel);

    m_dnf_logo = std::make_unique<Button>(StringContant::buttonName + "dnfLogo",
                                          m_shader,
                                          Button::ButtonTextureType::Logo);

    // Move components - main page
    m_play_button->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_play_button->getTextureGeo().x / 2.0f),
                  m_buttons_offset + m_exit_button->getTextureGeo().y
                      + m_control_button->getTextureGeo().y,
                  0.0f));
    m_control_button->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_control_button->getTextureGeo().x / 2.0f),
                  m_buttons_offset + m_control_button->getTextureGeo().y,
                  0.0f));
    m_exit_button->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_exit_button->getTextureGeo().x / 2.0f),
                  m_buttons_offset,
                  0.0f));
    m_dnf_logo->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_dnf_logo->getTextureGeo().x / 2.0f),
                  m_frame_buffer_height - m_dnf_logo->getTextureGeo().y - m_dnf_logo_offset,
                  0.0f));

    // Move components - control page
    m_back_button->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_back_button->getTextureGeo().x / 2.0f),
                  m_buttons_offset - 100.0f,
                  0.0f));
    m_move_label->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_move_label->getTextureGeo().x / 2.0f) - 350.0f,
                  m_frame_buffer_height - m_move_label->getTextureGeo().y - 50.0f,
                  0.0f));
    m_arrow_key->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_arrow_key->getTextureGeo().x / 2.0f),
                  m_move_label->getButtonDy() - m_arrow_key->getTextureGeo().y - 50.0f,
                  0.0f));
    m_attack_label->translate(
        glm::vec3(m_move_label->getButtonDx(), m_arrow_key->getButtonDy() - 150.0f, 0.0f));
    m_z_key->translate(
        glm::vec3(m_attack_label->getButtonDx() + (m_attack_label->getTextureGeo().x / 2.0f)
                      - ((m_z_key->getTextureGeo().x + m_x_key->getTextureGeo().x) / 2.0f),
                  m_attack_label->getButtonDy() - 100.0f,
                  0.0f));
    m_x_key->translate(glm::vec3(m_z_key->getButtonDx() + m_z_key->getTextureGeo().x,
                                 m_z_key->getButtonDy(),
                                 0.0f));
    m_interact_label->translate(glm::vec3(m_frame_buffer_width / 2.0f
                                              - (m_interact_label->getTextureGeo().x / 2.0f)
                                              + 350.0f,
                                          m_attack_label->getButtonDy(),
                                          0.0f));
    m_mouse_left->translate(glm::vec3(m_interact_label->getButtonDx()
                                          + (m_interact_label->getTextureGeo().x / 2.0f
                                             - m_mouse_left->getTextureGeo().x / 2.0f),
                                      m_interact_label->getButtonDy() - 150.0f,
                                      0.0f));
}

void
SceneZero::prepareInitialDisplay()
{
    m_scene_zero_root_node->addChild(m_scene_zero_background.get());
    m_scene_zero_root_node->addChild(m_play_button.get());
    m_scene_zero_root_node->addChild(m_control_button.get());
    m_scene_zero_root_node->addChild(m_exit_button.get());
    m_scene_zero_root_node->addChild(m_dnf_logo.get());
}

bool
SceneZero::processHover(const glm::vec2& mousePos)
{
    bool changed {false};
    CurrentButtonHoverInScene thisHover = CurrentButtonHoverInScene::None;

    switch (m_current_scene_stage) {
    case SceneZero::SceneZeroStage::MainPage: {
        bool controlHover = m_control_button->checkOnTop(mousePos);
        bool exitHover = m_exit_button->checkOnTop(mousePos);
        bool playerHover = m_play_button->checkOnTop(mousePos);

        if (controlHover)
            thisHover = CurrentButtonHoverInScene::Control;
        if (exitHover)
            thisHover = CurrentButtonHoverInScene::Exit;
        if (playerHover)
            thisHover = CurrentButtonHoverInScene::Play;

        break;
    }
    case SceneZero::SceneZeroStage::ControlPage: {
        bool backHover = m_back_button->checkOnTop(mousePos);

        if (backHover)
            thisHover = CurrentButtonHoverInScene::Back;
        break;
    }
    default:
        break;
    }

    if (thisHover != m_current_button_hover) {
        m_current_button_hover = thisHover;
        changed = true;
    }

    return changed && m_current_button_hover != CurrentButtonHoverInScene::None;
}

Scene::SceneEvents
SceneZero::processClick()
{
    if (m_exit_button->checkOnTop()) {
        return Scene::SceneEvents::QuitGame;
    } else if (m_play_button->checkOnTop()) {
        m_scene_zero_root_node->cleanChild();

        return Scene::SceneEvents::SceneTransit;
    } else if (m_control_button->checkOnTop()) {
        m_current_scene_stage = SceneZeroStage::ControlPage;

        m_scene_zero_root_node->removeChild(m_play_button.get());
        m_scene_zero_root_node->removeChild(m_control_button.get());
        m_scene_zero_root_node->removeChild(m_exit_button.get());
        m_scene_zero_root_node->removeChild(m_dnf_logo.get());

        m_scene_zero_root_node->addChild(m_back_button.get());
        m_scene_zero_root_node->addChild(m_attack_label.get());
        m_scene_zero_root_node->addChild(m_move_label.get());
        m_scene_zero_root_node->addChild(m_interact_label.get());
        m_scene_zero_root_node->addChild(m_arrow_key.get());
        m_scene_zero_root_node->addChild(m_mouse_left.get());
        m_scene_zero_root_node->addChild(m_z_key.get());
        m_scene_zero_root_node->addChild(m_x_key.get());

        // Reset hover state
        m_control_button->checkOnTop(glm::vec2(0.0f));

        return Scene::SceneEvents::None;
    } else if (m_back_button->checkOnTop()) {
        m_current_scene_stage = SceneZeroStage::MainPage;

        m_scene_zero_root_node->addChild(m_play_button.get());
        m_scene_zero_root_node->addChild(m_control_button.get());
        m_scene_zero_root_node->addChild(m_exit_button.get());
        m_scene_zero_root_node->addChild(m_dnf_logo.get());

        m_scene_zero_root_node->removeChild(m_back_button.get());
        m_scene_zero_root_node->removeChild(m_attack_label.get());
        m_scene_zero_root_node->removeChild(m_move_label.get());
        m_scene_zero_root_node->removeChild(m_interact_label.get());
        m_scene_zero_root_node->removeChild(m_arrow_key.get());
        m_scene_zero_root_node->removeChild(m_mouse_left.get());
        m_scene_zero_root_node->removeChild(m_z_key.get());
        m_scene_zero_root_node->removeChild(m_x_key.get());

        // Reset hover state
        m_back_button->checkOnTop(glm::vec2(0.0f));

        return Scene::SceneEvents::None;
    }

    return Scene::SceneEvents::None;
}