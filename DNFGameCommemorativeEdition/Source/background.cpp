#include "background.hpp"
#include "gamewindow.hpp"
#include "constant.hpp"

#include "glerrorcheck.hpp"

#include <gtc/matrix_transform.hpp>

// clang-format off

// Background vertex at the live area origin with texture coord
static const GLfloat background_vertex_buffer_data[] =
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

Background::Background(const std::string& name,
                       ShaderProgram* shader,
                       GLfloat width,
                       GLfloat height,
                       BackgroundTextureType textureType)
    : GeometryNode(name)
    , m_shader(shader)
{
    // Load texture
    switch (textureType) {
    case BackgroundTextureType::Mid:
        m_texture = Texture(TexturePath::backgoundMidPath);
        break;
    case BackgroundTextureType::Far:
        m_texture = Texture(TexturePath::backgroundFarPath);
        break;
    case BackgroundTextureType::SceneZero:
        m_texture = Texture(TexturePath::sceneZeroBackgroundPath);
        break;
    default:
        break;
    }

    m_texture.loadTexture();

    if (width == 0)
        width = static_cast<float>(m_texture.getTextureWidth());

    m_plane_width = width;
    m_plane_height = height;

    // S * T * R * T^-1
    // Scale will have no effect on translation
    m_trans = glm::scale(m_trans, glm::vec3(width, height, 0.0f));
    m_trans = glm::translate(m_trans, glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = glm::rotate(m_trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_trans = glm::translate(m_trans, glm::vec3(-0.5f, -0.5f, 0.0f));

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_background_vao);
    glBindVertexArray(m_background_vao);

    // Create the cube vertex buffer
    glGenBuffers(1, &m_background_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_background_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(background_vertex_buffer_data),
                 background_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(background_vertex_buffer_data[0]) * 5,
                          nullptr);

    // texture coord
    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(background_vertex_buffer_data[0]) * 5,
                          (void*) (sizeof(background_vertex_buffer_data[0]) * 3));

    // Reset state to prevent rogue code from messing with *my*
    // stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
Background::draw()
{
    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_texture.useTexture();

    glBindVertexArray(m_background_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
