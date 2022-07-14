#include "scenezero.hpp"
#include "background.hpp"
#include "button.hpp"
#include "game.hpp"

// Offset
const static float m_dnf_logo_offset {60.0f};
const static float m_buttons_offset {120.0f};

SceneZero::SceneZero(ShaderProgram* shader, GLfloat frameBufferWidth, GLfloat frameBufferHeight)
    : m_shader(shader)
    , m_frame_buffer_width(frameBufferWidth)
    , m_frame_buffer_height(frameBufferHeight)
    , m_scene_zero_root_node(std::make_unique<SceneNode>(StringContant::sceneZeroRootNodeName))
    , m_current_button_hover(CurrentButtonHoverInScene::None)
{
    construct();
}

SceneZero::~SceneZero() {}

void
SceneZero::construct()
{
    m_scene_zero_background = new Background(StringContant::sceneZeroBackgroundName,
                                             m_shader,
                                             m_frame_buffer_width,
                                             m_frame_buffer_height,
                                             Background::BackgroundTextureType::SceneZero);

    m_play_button = new Button(StringContant::buttonName + "play",
                               m_shader,
                               Button::ButtonTextureType::PlayButton);

    m_exit_button = new Button(StringContant::buttonName + "exit",
                               m_shader,
                               Button::ButtonTextureType::ExitButton);

    m_dnf_logo = new Button(StringContant::buttonName + "dnfLogo",
                            m_shader,
                            Button::ButtonTextureType::Logo);

    m_play_button->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_play_button->getTextureGeo().x / 2.0f),
                  m_buttons_offset + m_exit_button->getTextureGeo().y,
                  0.0f));
    m_exit_button->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_exit_button->getTextureGeo().x / 2.0f),
                  m_buttons_offset,
                  0.0f));
    m_dnf_logo->translate(
        glm::vec3(m_frame_buffer_width / 2.0f - (m_dnf_logo->getTextureGeo().x / 2.0f),
                  m_frame_buffer_height - m_dnf_logo->getTextureGeo().y - m_dnf_logo_offset,
                  0.0f));
}

void
SceneZero::prepareInitialDisplay()
{
    m_scene_zero_root_node->addChild(m_scene_zero_background);
    m_scene_zero_root_node->addChild(m_play_button);
    m_scene_zero_root_node->addChild(m_exit_button);
    m_scene_zero_root_node->addChild(m_dnf_logo);
}

bool
SceneZero::processHover(const glm::vec2& mousePos)
{
    bool changed {false};
    bool exitHover = m_exit_button->checkOnTop(mousePos);
    bool playerHover = m_play_button->checkOnTop(mousePos);

    CurrentButtonHoverInScene thisHover = exitHover
                                              ? CurrentButtonHoverInScene::Exit
                                              : (playerHover ? CurrentButtonHoverInScene::Play
                                                             : CurrentButtonHoverInScene::None);
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
        return Scene::SceneEvents::SceneTransit;
    }

    return Scene::SceneEvents::None;
}