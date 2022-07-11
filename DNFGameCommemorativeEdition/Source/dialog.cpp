#include "dialog.hpp"
#include "gamewindow.hpp"
#include "constant.hpp"

#include "glerrorcheck.hpp"

#include <gtc/matrix_transform.hpp>

// clang-format off

// Dialog vertex at the live area origin with texture coord
static const GLfloat dialog_vertex_buffer_data[] =
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

static const float m_dialog_y_offset {20.0f};

// clang-format on

Dialog::Dialog(ShaderProgram* shader, GLfloat windowWidth, GLfloat windowHeight)
    : GeometryNode(StringContant::dialogUIName)
    , m_shader(shader)
    , m_window_width(windowWidth)
    , m_window_height(windowHeight)
{
    // Load texture
    m_texture = Texture(TexturePath::dialogUIPath);
    m_texture.loadTexture();

    m_plane_width = m_window_width * DialogSize::dialogWidthRatio;
    m_plane_height = DialogSize::dialogHeight;

    // S * T * R * T^-1
    // Scale will have no effect on translation
    m_trans = glm::translate(m_trans,
                             glm::vec3(m_window_width / 2.0f - m_plane_width / 2.0f,
                                       m_dialog_y_offset,
                                       0.0f));

    m_trans = glm::scale(m_trans, glm::vec3(m_plane_width, m_plane_height, 0.0f));
    m_trans = glm::translate(m_trans, glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = glm::rotate(m_trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_trans = glm::translate(m_trans, glm::vec3(-0.5f, -0.5f, 0.0f));

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_dialog_vao);
    glBindVertexArray(m_dialog_vao);

    // Create the floor vertex buffer
    glGenBuffers(1, &m_dialog_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_dialog_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(dialog_vertex_buffer_data),
                 dialog_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(dialog_vertex_buffer_data[0]) * 5,
                          nullptr);

    // texture coord
    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(dialog_vertex_buffer_data[0]) * 5,
                          (void*) (sizeof(dialog_vertex_buffer_data[0]) * 3));

    // Reset state to prevent rogue code from messing with *my* stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
Dialog::draw()
{
    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_texture.useTexture();

    glBindVertexArray(m_dialog_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

glm::vec2
Dialog::getOriginTrans()
{
    return glm::vec2(m_window_width / 2.0f - m_plane_width / 2.0f, m_dialog_y_offset);
}
