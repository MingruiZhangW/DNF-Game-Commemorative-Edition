#include "background.hpp"
#include "gamewindow.hpp"

#include "glerrorcheck.hpp"

#include <gtc/matrix_transform.hpp>

// clang-format off

// Floor vertex at the live area origin with texture coord
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

Background::Background(const std::string& name)
    : GeometryNode(name)
{}

void
Background::init(ShaderProgram* shader, GLfloat width, GLfloat height)
{
    m_shader = shader;

    m_background_width = width;
    m_background_height = height;

    m_trans = glm::scale(m_trans, glm::vec3(width, height, 0.0f));

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

    // Init texture
    m_grass_texture = Texture("Resource/Texture/Background/BackgroundFar.png");
    m_grass_texture.loadTexture();

    CHECK_GL_ERRORS;
}
void
Background::draw()
{
    m_grass_texture.useTexture();

    glBindVertexArray(m_background_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);
}
