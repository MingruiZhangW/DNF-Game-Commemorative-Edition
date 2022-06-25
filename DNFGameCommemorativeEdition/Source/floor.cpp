#include "floor.hpp"
#include "gamewindow.hpp"

#include "glerrorcheck.hpp"

// clang-format off

// Floor vertex at the live area origin with texture coord
static const GLfloat floor_vertex_buffer_data[] =
{
        // triangle 1
        // x,     y,    z,    u,    v,
		0.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	    0.0f,  0.0f, 0.0f, 1.0f, 0.0f,
        // triangle 2
	    1.0f,  0.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	    0.0f,  0.0f, 0.0f, 1.0f, 0.0f
};

// clang-format on

Floor::Floor() {}

void
Floor::init(ShaderProgram* shader)
{
    m_shader = shader;

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_floor_vao);
    glBindVertexArray(m_floor_vao);

    // Create the cube vertex buffer
    glGenBuffers(1, &m_floor_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_floor_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(floor_vertex_buffer_data),
                 floor_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(floor_vertex_buffer_data[0]) * 5,
                          nullptr);

    // texture coord
    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(floor_vertex_buffer_data[0]) * 5,
                          (void*) (sizeof(floor_vertex_buffer_data[0]) * 3));

    // Reset state to prevent rogue code from messing with *my*
    // stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Init texture
    m_grass_texture = Texture("Resource/Texture/Tiles/grassTile.png");
    m_grass_texture.loadTexture();

    CHECK_GL_ERRORS;
}
void
Floor::draw()
{
    m_grass_texture.useTexture();

    glBindVertexArray(m_floor_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);
}
