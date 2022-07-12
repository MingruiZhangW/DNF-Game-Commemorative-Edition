#include "dialogchar.hpp"
#include "gamewindow.hpp"

#include "glerrorcheck.hpp"

#include <gtc/matrix_transform.hpp>

DialogChar::DialogChar(const std::string& name,
                       ShaderProgram* shader,
                       const Fonts::CharacterInfo& charInfo,
                       GLfloat scale,
                       CharType charType)
    : GeometryNode(name)
    , m_shader(shader)
    , m_char_type(charType)
    , m_char_info(charInfo)
    , m_char_scale_x(scale)
    , m_char_scale_y(scale)
{
    // For english charaters only, we need to make the plane size varies.
    // Chinese charaters will always remain the same.
    if (charType == CharType::English) {
        m_char_scale_y = m_char_scale_y
                         * (static_cast<float>(charInfo.textureHeight)
                            / static_cast<float>(Fonts::fontTextSize));

        m_char_scale_x = m_char_scale_x
                         * (static_cast<float>(charInfo.textureWidth)
                            / static_cast<float>(Fonts::fontTextSize));
    }

    // S * T * R * T^-1
    // Scale will have no effect on translation
    m_trans = glm::scale(m_trans, glm::vec3(m_char_scale_x, m_char_scale_y, 0.0f));
    m_trans = glm::translate(m_trans, glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = glm::rotate(m_trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_trans = glm::translate(m_trans, glm::vec3(-0.5f, -0.5f, 0.0f));

    // clang-format off
    GLfloat floor_vertex_buffer_data[] =
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

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_dialog_char_vao);
    glBindVertexArray(m_dialog_char_vao);

    // Create the floor vertex buffer
    // Here we need to create vbo dynamically, just allocate the memory first.
    // 5 * 6 -> x, y ,z, u ,v and 6 vertices
    glGenBuffers(1, &m_dialog_char_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_dialog_char_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * 6, floor_vertex_buffer_data, GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glVertexAttribPointer(m_position_attrib_pos, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);

    // texture coord
    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * 5,
                          (void*) (sizeof(float) * 3));

    m_draw_text_id = m_shader->getUniformLocation("drawText");

    // Reset state to prevent rogue code from messing with *my* stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
DialogChar::draw()
{
    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Texture Unit fo sampler2D in shader
    glActiveTexture(GL_TEXTURE0);
    // Bind texture to texture Unit
    glBindTexture(GL_TEXTURE_2D, m_char_info.textureID);

    glBindVertexArray(m_dialog_char_vao);
    // Draw text uniform here
    glUniform1i(m_draw_text_id, true);

    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);

    glUniform1i(m_draw_text_id, false);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
