#include "button.hpp"
#include "gamewindow.hpp"
#include "constant.hpp"

#include "glerrorcheck.hpp"

#include <gtc/matrix_transform.hpp>

// clang-format off

// Button vertex at the live area origin with texture coord
static const GLfloat button_vertex_buffer_data[] =
{
    // triangle 1
    // x,    y,    z,    u,    v,
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // triangle 2
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

// clang-format on

// Hover offset const
static const float m_buttonhover_x_offset {10.0f};
static const float m_button_hover_y_offset {10.0f};

Button::Button(const std::string& name, ShaderProgram* shader, ButtonTextureType textureType)
    : GeometryNode(name)
    , m_shader(shader)
    , m_current_button_t_type(textureType)
    , m_button_is_hovered(false)
    , m_button_dx(0.0f)
    , m_button_dy(0.0f)
{
    // Load texture
    switch (textureType) {
    case ButtonTextureType::ExitButton:
        m_texture_normal = Texture(TexturePath::exitButtonNormalUIPath);
        m_texture_hover = Texture(TexturePath::exitButtonHoverUIPath);

        m_texture_hover.loadTexture();
        break;
    case ButtonTextureType::PlayButton:
        m_texture_normal = Texture(TexturePath::playButtonNormalUIPath);
        m_texture_hover = Texture(TexturePath::playButtonHoverUIPath);

        m_texture_hover.loadTexture();
        break;
    case ButtonTextureType::Logo:
        m_texture_normal = Texture(TexturePath::dnfLogoPath);
        break;
    default:
        break;
    }

    m_texture_normal.loadTexture();

    m_plane_width = static_cast<float>(m_texture_normal.getTextureWidth());
    m_plane_height = static_cast<float>(m_texture_normal.getTextureHeight());

    // S * T * R * T^-1
    // Scale will have no effect on translation
    m_trans = glm::scale(m_trans, glm::vec3(m_plane_width, m_plane_height, 0.0f));
    m_trans = glm::translate(m_trans, glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = glm::rotate(m_trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_trans = glm::translate(m_trans, glm::vec3(-0.5f, -0.5f, 0.0f));

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_button_vao);
    glBindVertexArray(m_button_vao);

    // Create the cube vertex buffer
    glGenBuffers(1, &m_button_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_button_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(button_vertex_buffer_data),
                 button_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(button_vertex_buffer_data[0]) * 5,
                          nullptr);

    // texture coord
    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(button_vertex_buffer_data[0]) * 5,
                          (void*) (sizeof(button_vertex_buffer_data[0]) * 3));

    // Reset state to prevent rogue code from messing with *my*
    // stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
Button::translate(const glm::vec3& amount)
{
    m_button_dx = m_button_dx + amount.x;
    m_button_dy = m_button_dy + amount.y;

    SceneNode::translate(amount);
}

bool
Button::checkOnTop()
{
    return m_button_is_hovered;
}

bool
Button::checkOnTop(const glm::vec2& mousePos)
{
    if (m_buttonhover_x_offset + m_button_dx < mousePos.x
        && mousePos.x < m_button_dx + m_texture_normal.getTextureWidth() - m_buttonhover_x_offset) {
        if (m_button_hover_y_offset + m_button_dy < mousePos.y
            && mousePos.y
                   < m_button_dy + m_texture_normal.getTextureHeight() - m_button_hover_y_offset) {
            m_button_is_hovered = true;
            return m_button_is_hovered;
        }
    }

    m_button_is_hovered = false;
    return m_button_is_hovered;
}

void
Button::draw()
{
    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (m_current_button_t_type) {
    case Button::ButtonTextureType::ExitButton:
    case Button::ButtonTextureType::PlayButton:
        switch (m_button_is_hovered) {
        case false:
            m_texture_normal.useTexture();
            break;
        case true:
            m_texture_hover.useTexture();
            break;
        }
        break;
    case Button::ButtonTextureType::Logo:
        m_texture_normal.useTexture();
        break;
    default:
        break;
    }

    glBindVertexArray(m_button_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
